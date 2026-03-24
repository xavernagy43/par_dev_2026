__kernel void add(__global const float* a,
                  __global const float* b,
                  __global float* res) {
    int i = get_global_id(0);
    res[i] = a[i] + b[i];
}