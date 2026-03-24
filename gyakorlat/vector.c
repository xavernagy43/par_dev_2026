#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

char* loadKernelSource(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    char* source = (char*)malloc(size + 1);
    fread(source, 1, size, file);
    source[size] = '\0';
    fclose(file);
    return source;
}

float* vector_add(const float* a, const float* b, int N) {
    float* result = (float*)malloc(sizeof(float) * N);
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

    char* source = loadKernelSource("kernel/vector_kernel.cl");
    if (!source) {
        free(result);
        return NULL;
    }

    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &err);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "add", &err);

    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, (void*)a, &err);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, (void*)b, &err);
    cl_mem bufferRes = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N, NULL, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferRes);

    size_t globalSize = N;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufferRes, CL_TRUE, 0, sizeof(float) * N, result, 0, NULL, NULL);

    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferRes);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(source);

    return result;
}

int main() {
    int N = 3;
    float a[3] = {1.5f, 2.2f, -1.0f};
    float b[3] = {0.5f, -2.2f, 3.0f};

    float* result = vector_add(a, b, N);
    if (!result) {
        printf("Hiba: nem sikerult a vector_add.\n");
        return -1;
    }

    printf("OpenCL eredmeny:\n");
    for (int i = 0; i < N; i++) printf("[%d] = %.2f\n", i, result[i]);

    printf("\nSzekvencialis ellenorzes:\n");
    for (int i = 0; i < N; i++) printf("[%d] = %.2f\n", i, a[i] + b[i]);

    free(result);
    return 0;
}