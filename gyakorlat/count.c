#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

int* count_occurrences(const int* input, int N) {
    int* output = (int*)malloc(sizeof(int) * N);
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

    char* source = loadKernelSource("kernel/count_kernel.cl");
    if (!source) {
        free(output);
        return NULL;
    }

    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &err);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "count_occurrences", &err);

    cl_mem bufferInput = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * N, (void*)input, &err);
    cl_mem bufferOutput = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * N, NULL, &err);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferInput);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferOutput);
    clSetKernelArg(kernel, 2, sizeof(int), &N);

    size_t globalSize = N;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);
    clEnqueueReadBuffer(queue, bufferOutput, CL_TRUE, 0, sizeof(int) * N, output, 0, NULL, NULL);

    clReleaseMemObject(bufferInput);
    clReleaseMemObject(bufferOutput);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(source);

    return output;
}

int main() {
    const int N = 20;
    int input[N];

    srand((unsigned int)time(NULL));
    for (int i = 0; i < N; i++) {
        input[i] = rand() % 20 + 1;
    }

    int* counts = count_occurrences(input, N);
    if (!counts) {
        printf("Hiba: nem sikerult a szamolas.\n");
        return -1;
    }

    printf("Tomb: ");
    for (int i = 0; i < N; i++) printf("%d ", input[i]);
    printf("\nElofordulasok: ");
    for (int i = 0; i < N; i++) printf("%d ", counts[i]);
    printf("\n");

    //Egyediseg ellenorzese
    int unique = 1;
    for (int i = 0; i < N; i++) {
        if (counts[i] > 1) {
            unique = 0;
            break;
        }
    }
    
    if (unique) printf("Minden ertek egyedi.\n");
    else printf("Nem minden ertek egyedi.\n");

    free(counts);
    return 0;
}