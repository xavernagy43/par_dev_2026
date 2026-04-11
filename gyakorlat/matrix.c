#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(err, msg) if (err != CL_SUCCESS) { printf("%s: %d\n", msg, err); exit(1); }

void initOpenCL();
void transposeMatrix(float* A, float* B, int rows, int cols);
void multiplyMatrix(float* A, float* B, float* C, int Arows, int Acols, int Bcols);
void columnSum(float* A, float* out, int rows, int cols);
void rowSumResult(float* A, float* out, int rows, int cols);
void printMatrix(float* M, int rows, int cols);

const char* kernelSource =
"__kernel void transpose(__global float* A, __global float* B, int rows, int cols) {\n"
"    int r = get_global_id(0);\n"
"    int c = get_global_id(1);\n"
"    if (r < rows && c < cols)\n"
"        B[c * rows + r] = A[r * cols + c];\n"
"}\n"
"\n"
"__kernel void multiply(__global float* A, __global float* B, __global float* C,\n"
"                       int Arows, int Acols, int Bcols) {\n"
"    int r = get_global_id(0);\n"
"    int c = get_global_id(1);\n"
"    if (r < Arows && c < Bcols) {\n"
"        float sum = 0.0f;\n"
"        for (int k = 0; k < Acols; k++)\n"
"            sum += A[r * Acols + k] * B[k * Bcols + c];\n"
"        C[r * Bcols + c] = sum;\n"
"    }\n"
"}\n"
"\n"
"__kernel void columnSum(__global float* A, __global float* out, int rows, int cols) {\n"
"    int c = get_global_id(0);\n"
"    if (c < cols) {\n"
"        float sum = 0.0f;\n"
"        for (int r = 0; r < rows; r++)\n"
"            sum += A[r * cols + c];\n"
"        out[c] = sum;\n"
"    }\n"
"}\n"
"\n"
"__kernel void rowSum(__global float* A, __global float* out, int rows, int cols) {\n"
"    int r = get_global_id(0);\n"
"    if (r < rows) {\n"
"        float sum = 0.0f;\n"
"        for (int c = 0; c < cols; c++)\n"
"            sum += A[r * cols + c];\n"
"        out[r] = sum;\n"
"    }\n"
"}\n";

cl_context context;
cl_command_queue queue;
cl_program program;



int main() {
    initOpenCL();

    float A[6] = { 1,2,3, 4,5,6 }; // 2×3
    float B[6];
    float C[4];
    float colSum[3];
    float rowSum[2];

    transposeMatrix(A, B, 2, 3);
    multiplyMatrix(A, B, C, 2, 3, 2);
    columnSum(A, colSum, 2, 3);
    rowSumResult(A, rowSum, 2, 3);

    printf("A matrix:\n");
    printMatrix(A, 2, 3);

    printf("Transposed (B):\n");
    printMatrix(B, 3, 2);

    printf("Multiplication (C = A * B):\n");
    printMatrix(C, 2, 2);

    printf("Column sums:\n");
    for (int i = 0; i < 3; i++)
        printf("%6.2f ", colSum[i]);
    printf("\n\n");

    printf("Row sums:\n");
    for (int i = 0; i < 2; i++)
        printf("%6.2f ", rowSum[i]);
    printf("\n");

    return 0;
}

void initOpenCL() {
    cl_int err;

    cl_platform_id platform;
    cl_device_id device;

    err = clGetPlatformIDs(1, &platform, NULL);
    CHECK(err, "Platform error");

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    CHECK(err, "Device error");

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK(err, "Context error");

    queue = clCreateCommandQueue(context, device, 0, &err);
    CHECK(err, "Queue error");

    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, &err);
    CHECK(err, "Program create error");

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t logSize;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
        char* log = malloc(logSize);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log, NULL);
        printf("Build error:\n%s\n", log);
        free(log);
        exit(1);
    }
}

void transposeMatrix(float* A, float* B, int rows, int cols) {
    cl_int err;

    cl_kernel kernel = clCreateKernel(program, "transpose", &err);
    CHECK(err, "Kernel transpose");

    size_t sizeA = rows * cols * sizeof(float);
    size_t sizeB = rows * cols * sizeof(float);

    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeA, A, &err);
    cl_mem bufB = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeB, NULL, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    clSetKernelArg(kernel, 2, sizeof(int), &rows);
    clSetKernelArg(kernel, 3, sizeof(int), &cols);

    size_t global[2] = { rows, cols };
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, bufB, CL_TRUE, 0, sizeB, B, 0, NULL, NULL);

    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseKernel(kernel);
}

void multiplyMatrix(float* A, float* B, float* C, int Arows, int Acols, int Bcols) {
    cl_int err;

    cl_kernel kernel = clCreateKernel(program, "multiply", &err);
    CHECK(err, "Kernel multiply");

    size_t sizeA = Arows * Acols * sizeof(float);
    size_t sizeB = Acols * Bcols * sizeof(float);
    size_t sizeC = Arows * Bcols * sizeof(float);

    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeA, A, &err);
    cl_mem bufB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeB, B, &err);
    cl_mem bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeC, NULL, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);
    clSetKernelArg(kernel, 3, sizeof(int), &Arows);
    clSetKernelArg(kernel, 4, sizeof(int), &Acols);
    clSetKernelArg(kernel, 5, sizeof(int), &Bcols);

    size_t global[2] = { Arows, Bcols };
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, sizeC, C, 0, NULL, NULL);

    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseKernel(kernel);
}

void columnSum(float* A, float* out, int rows, int cols) {
    cl_int err;

    cl_kernel kernel = clCreateKernel(program, "columnSum", &err);
    CHECK(err, "Kernel columnSum");

    size_t sizeA = rows * cols * sizeof(float);
    size_t sizeOut = cols * sizeof(float);

    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeA, A, &err);
    cl_mem bufOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeOut, NULL, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufOut);
    clSetKernelArg(kernel, 2, sizeof(int), &rows);
    clSetKernelArg(kernel, 3, sizeof(int), &cols);

    size_t global = cols;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, bufOut, CL_TRUE, 0, sizeOut, out, 0, NULL, NULL);

    clReleaseMemObject(bufA);
    clReleaseMemObject(bufOut);
    clReleaseKernel(kernel);
}

void rowSumResult(float* A, float* out, int rows, int cols) {
    cl_int err;

    cl_kernel kernel = clCreateKernel(program, "rowSum", &err);
    CHECK(err, "Kernel rowSum");

    size_t sizeA = rows * cols * sizeof(float);
    size_t sizeOut = rows * sizeof(float);

    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeA, A, &err);
    cl_mem bufOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeOut, NULL, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufOut);
    clSetKernelArg(kernel, 2, sizeof(int), &rows);
    clSetKernelArg(kernel, 3, sizeof(int), &cols);

    size_t global = rows;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, bufOut, CL_TRUE, 0, sizeOut, out, 0, NULL, NULL);

    clReleaseMemObject(bufA);
    clReleaseMemObject(bufOut);
    clReleaseKernel(kernel);
}

void printMatrix(float* M, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%6.2f ", M[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}