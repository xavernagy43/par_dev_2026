#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHECK(err, msg) if (err != CL_SUCCESS) { printf("%s: %d\n", msg, err); exit(1); }

cl_context context;
cl_command_queue queue;
cl_program program;
cl_device_id device;
cl_int err;

char* loadKernelSource(const char* filename);
void computeFrequencies(int* A, int N);
void initOpenCL();

int main() {
    initOpenCL();

    char* source = loadKernelSource("kernel/rand_freq_kernel.cl");
    if (!source) return 1;

    program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &err);
    CHECK(err, "Program create");

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

    free(source);

    int N = 20;
    int* A = (int*)malloc(N * sizeof(int));

    // RANDOM feltöltés (0–100)
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        A[i] = rand() % 101;
    }

    // KIÍRÁS (ellenőrzéshez)
    printf("Tömb:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", A[i]);
    }
    printf("\n\n");

    computeFrequencies(A, N);

    free(A);
    return 0;
}

void initOpenCL() {
    cl_platform_id platform;

    err = clGetPlatformIDs(1, &platform, NULL);
    CHECK(err, "Platform");

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    CHECK(err, "Device");

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK(err, "Context");

    queue = clCreateCommandQueue(context, device, 0, &err);
    CHECK(err, "Queue");
}

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

void computeFrequencies(int* A, int N) {
    cl_kernel kernel = clCreateKernel(program, "freqKernel", &err);
    CHECK(err, "Kernel");

    size_t sizeA = N * sizeof(int);
    size_t sizeF = 101 * sizeof(int);

    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeA, A, &err);
    CHECK(err, "Buffer A");

    cl_mem bufF = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeF, NULL, &err);
    CHECK(err, "Buffer F");

    int zero[101] = {0};
    clEnqueueWriteBuffer(queue, bufF, CL_TRUE, 0, sizeF, zero, 0, NULL, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufF);
    clSetKernelArg(kernel, 2, sizeof(int), &N);

    size_t global = N;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global, NULL, 0, NULL, NULL);

    int freq[101];
    clEnqueueReadBuffer(queue, bufF, CL_TRUE, 0, sizeF, freq, 0, NULL, NULL);

    printf("Gyakorisagok:\n");
    for (int i = 0; i <= 100; i++) {
        if (freq[i] > 0)
            printf("%3d: %d\n", i, freq[i]);
    }

    clReleaseMemObject(bufA);
    clReleaseMemObject(bufF);
    clReleaseKernel(kernel);
}