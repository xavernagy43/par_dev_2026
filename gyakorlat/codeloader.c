#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

char* loadKernelSource(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Hiba: nem lehet megnyitni a fajlt!\n");
        return NULL;
    }

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
    const int N = 10;
    int host_buffer[N];

    cl_int err;

    cl_platform_id platform;
    cl_device_id device;

    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

    // Kernel betöltése
    char* source = loadKernelSource("kernel/hello_kernel.cl");
    if (!source) return -1;

    cl_program program = clCreateProgramWithSource(context, 1,
        (const char**)&source, NULL, &err);

    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        char* log = (char*)malloc(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

        printf("Build error:\n%s\n", log);
        free(log);
        return -1;
    }

    cl_kernel kernel = clCreateKernel(program, "hello_kernel", &err);

    cl_mem device_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
        sizeof(int) * N, NULL, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &device_buffer);
    clSetKernelArg(kernel, 1, sizeof(int), &N);

    size_t globalSize = N;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, device_buffer, CL_TRUE, 0,
        sizeof(int) * N, host_buffer, 0, NULL, NULL);

    for (int i = 0; i < N; i++) {
        printf("[%d] = %d\n", i, host_buffer[i]);
    }

    clReleaseMemObject(device_buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(source);

    return 0;
}