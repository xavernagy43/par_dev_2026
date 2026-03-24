__kernel void count_occurrences(__global const int* input,
                                __global int* output,
                                int N) {
    int i = get_global_id(0);
    int count = 0;
    for (int j = 0; j < N; j++) {
        if (input[j] == input[i]) count++;
    }
    output[i] = count;
}