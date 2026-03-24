/*__kernel void hello_kernel(__global int* buffer, int n) {
    int id = get_global_id(0);
    if (id < n) buffer[id] = 11;
} */

__kernel void hello_kernel(__global int* buffer, int n) {
    int gid = get_global_id(0);   //globális index
    int lid = get_local_id(0);    //lokális index 

    if (gid < n) {
        //Globális + lokális index 
        buffer[gid] = gid * 10 + lid;
    }

    barrier(CLK_GLOBAL_MEM_FENCE);

    //Visszafelé sorrend
    if (gid < n / 2) {
        int temp = buffer[gid];
        buffer[gid] = buffer[n - 1 - gid];
        buffer[n - 1 - gid] = temp;
    }

    barrier(CLK_GLOBAL_MEM_FENCE);

    //Páros/páratlan csere
    if (gid % 2 == 0 && gid + 1 < n) {
        int tmp = buffer[gid];
        buffer[gid] = buffer[gid + 1];
        buffer[gid + 1] = tmp;
    }
}