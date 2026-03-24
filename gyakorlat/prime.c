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

int main() {
    int number = 23; //Ellenorzendo szam
    int flag = 0;

    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

    char* source = loadKernelSource("kernel/prime_kernel.cl");
    if (!source) return -1;

    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &err);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "prime_check", &err);

    cl_mem bufferNumber = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &number, &err);
    cl_mem bufferFlag = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int), &flag, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferNumber);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferFlag);

    size_t globalSize = number - 2;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufferFlag, CL_TRUE, 0, sizeof(int), &flag, 0, NULL, NULL);

    if (flag == 0) printf("%d prim szam.\n", number);
    else printf("%d nem prim szam.\n", number);

    clReleaseMemObject(bufferNumber);
    clReleaseMemObject(bufferFlag);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(source);

    return 0;
}