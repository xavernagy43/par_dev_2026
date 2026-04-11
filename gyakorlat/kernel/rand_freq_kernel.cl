__kernel void freqKernel(__global int* A, __global int* freq, int N) {
    int i = get_global_id(0);
    if (i < N) {
        int v = A[i];
        freq[v] = freq[v] + 1;
    }
}
