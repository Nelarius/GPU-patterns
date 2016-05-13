# version 430 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rg32f, binding = 1) uniform imageBuffer rdGrid;
layout(location = 3) uniform int u_width;
layout(location = 4) uniform int u_height;

vec4 vonNeumann(int index, int step1, int step2) {
    vec4 newElement = imageLoad(rdGrid, index);
    vec4 element1 = imageLoad(rdGrid, step1);
    vec4 element2 = imageLoad(rdGrid, step2);
    
    float act1 = element1.z;
    float inh1 = element1.w;
    float act2 = element2.z;
    float inh2 = element2.w;

    newElement.z = 0.33333 * (4.0 * act1 - act2);
    newElement.w = 0.33333 * (4.0 * inh1 - inh2);
    return newElement;
}

void main() {
    if (gl_GlobalInvocationID.x == 0) {
        int index = int(gl_GlobalInvocationID.y) * u_width;
        int right = index + 1;
        int right2 = index + 2;
        imageStore(rdGrid, index, vonNeumann(index, right, right2));
        return;
    }

    if (gl_GlobalInvocationID.x == u_width - 1) {
        int index = int(gl_GlobalInvocationID.y) * u_width + int(gl_GlobalInvocationID.x);
        int left = index - 1;
        int left2 = index - 2;
        imageStore(rdGrid, index, vonNeumann(index, left, left2));
        return;
    }

    if (gl_GlobalInvocationID.y == 0) {
        int index = int(gl_GlobalInvocationID.x);
        int top = index + u_width;
        int top2 = top + u_width;
        imageStore(rdGrid, index, vonNeumann(index, top, top2));
        return;
    }

    if (gl_GlobalInvocationID.y == u_height - 1) {
        int index = int(gl_GlobalInvocationID.y) * u_width + int(gl_GlobalInvocationID.x);
        int bottom = index - u_width;
        int bottom2 = bottom - u_width;
        imageStore(rdGrid, index, vonNeumann(index, bottom, bottom2));
        return;
    }
}
