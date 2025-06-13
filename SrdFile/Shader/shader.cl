    
__kernel void FragmentShader(__global uint* buffer, uint count) {
    uint id = get_global_id(0);
    if (id < count) {
        buffer[id] = 0xFF0000;
    }
}

__kernel void VertexShader(__global uint* buffer, uint count) {
    uint id = get_global_id(0);
    if (id < count) {
        buffer[id] = 0x0000FF;
    }
}