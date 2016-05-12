# version 430 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 1) uniform imageBuffer rdGrid;
layout(rgba32f, binding = 2) uniform imageBuffer rdGridPrevious;
layout(location = 3) uniform int u_width;

void main() {
    uint index = gl_GlobalInvocationID.y*u_width + gl_GlobalInvocationID.x;
    imageStore(rdGridPrevious, int(index), imageLoad(rdGrid, int(index)));
}
