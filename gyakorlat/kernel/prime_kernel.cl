__kernel void prime_check(__global const int* number,
                          __global int* flags) {
    int divisor = get_global_id(0) + 2;

    if (divisor < *number) {
        if (*number % divisor == 0) {
            flags[0] = 1;
        }
    }
}