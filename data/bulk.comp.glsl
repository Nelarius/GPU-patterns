# version 430 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 1) uniform imageBuffer rdGrid;
layout(rgba32f, binding = 2) uniform imageBuffer rdGridPrevious;
layout(location = 3) uniform int u_width;
layout(location = 4) uniform int u_height;
layout(location = 5) uniform float u_diffusivity;
layout(location = 6) uniform float u_deltaSpace;
layout(location = 7) uniform float u_deltaTime;
layout(location = 8) uniform float u_inhCoupling;
layout(location = 9) uniform float u_actCoupling;
layout(location = 10) uniform float u_inhBaseProduction;
layout(location = 11) uniform float u_actBaseProduction;
layout(location = 12) uniform float u_timeFactor;

void main() {
    // if (gl_GlobalInvocationID.x == 0) {
    //     int index = int(gl_GLobalInvocationID.y * u_width);
    //     int right = right + 1;
    //     int right2 = right + 1;
    //     vec4 newElement = imageLoad(rdGridPrevious, index);
    //     float act = newElement.z;
    //     float newAct = act + u_deltaTime * 0.33333 * (4.0 * imageLoad(rdGridPrevious, right).z - imageLoad(rdGridPrevious, right2).z);
    //     newElement.z = newAct;
    //     imageStore(rdGrid, index, newElement);
    //     return;
    // }
    // if (gl_GlobalInvocationID.x == u_height - 1) {
    //     int index = int(gl_GlobalInvocationID.y * u_width);
    //     int left = index - 1;
    //     int left2 = left - 1;
    //     vec4 newElement = imageLoad(rdGridPrevious, index);
    //     float act = newElement.z;
    //     float newAct = act + u_deltaTime * 0.3333 * (-4.0 * imageLoad(rdGridPrevious, left).z + imageLoad(rdGridPrevious, left2).z);
    //     newElement.z = newAct;
    //     imageStore(rdGrid, index, newElement);
    //     return;
    // }
    // if (gl_GLobalInvocationID.y == 0) {
    //     int index = int(gl_GlobalInvocationID.x);
    //     int top = index + u_width;
    //     int top2 = top + u_width;
    //     vec4 newElement = imageLoad();
    // }

    if (gl_GlobalInvocationID.x == 0 || gl_GlobalInvocationID.x == u_width - 1) {
        return;
    }
    if (gl_GlobalInvocationID.y == 0 || gl_GlobalInvocationID.y == u_height -1) {
        return;
    }
    int index = int(gl_GlobalInvocationID.y*u_width + gl_GlobalInvocationID.x);
    int top = int((gl_GlobalInvocationID.y + 1)*u_width + gl_GlobalInvocationID.x);
    int bottom = int((gl_GlobalInvocationID.y - 1)*u_width + gl_GlobalInvocationID.x);
    int left = int(gl_GlobalInvocationID.y*u_width + gl_GlobalInvocationID.x - 1);
    int right = int(gl_GlobalInvocationID.y*u_width + gl_GlobalInvocationID.x + 1);

    vec4 newElement = imageLoad(rdGridPrevious, index);
    float act = newElement.z;
    float actTop = imageLoad(rdGridPrevious, top).z;
    float actBottom = imageLoad(rdGridPrevious, bottom).z;
    float actLeft = imageLoad(rdGridPrevious, left).z;
    float actRight = imageLoad(rdGridPrevious, right).z;

    float inh = imageLoad(rdGridPrevious, index).w;
    float inhTop = imageLoad(rdGridPrevious, top).w;
    float inhBottom = imageLoad(rdGridPrevious, bottom).w;
    float inhLeft = imageLoad(rdGridPrevious, left).w;
    float inhRight = imageLoad(rdGridPrevious, right).w;

    float actNew = act + u_timeFactor * u_deltaTime * ( (u_diffusivity/(u_deltaSpace*u_deltaSpace)) * (actRight + actLeft + actTop + actBottom - 4*act) );// + u_actCoupling*(act*act / inh - act) + u_actBaseProduction);
    
    // float inhNew = inh + u_timeFactor * u_deltaTime * ( () );

    newElement.z = actNew;
    imageStore(rdGrid, index, newElement);
}
