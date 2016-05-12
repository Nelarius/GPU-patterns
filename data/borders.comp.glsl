# version 430 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rg32f, binding = 1) uniform imageBuffer rdGrid;
layout(location = 3) uniform int u_width;
layout(location = 4) uniform int u_height;

void main() {
    if (gl_GlobalInvocationID.x == 0) {
        //
        return;
    }

    if (gl_GlobalInvocationID.x == u_width - 1) {
        //
        return;
    }

    if (gl_GlobalInvocationID.y == 0) {
        //
        return;
    }

    if (gl_GlobalInvocationID.y == u_height - 1) {
        //
        return;
    }
}
