# version 430 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 1) uniform imageBuffer rdGrid;
layout(rgba32f, binding = 2) uniform imageBuffer rdGridPrevious;
layout(location = 3) uniform int u_width;
layout(location = 4) uniform int u_height;
layout(location = 5) uniform float u_inhDiffusivity;
layout(location = 6) uniform float u_actDiffusivity;
layout(location = 7) uniform float u_deltaSpace;
layout(location = 8) uniform float u_deltaTime;
layout(location = 9) uniform float u_inhCoupling;
layout(location = 10) uniform float u_actCoupling;
layout(location = 11) uniform float u_inhBaseProduction;
layout(location = 12) uniform float u_actBaseProduction;
layout(location = 13) uniform float u_timeFactor;

void main() {
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

    float inh = newElement.w;
    float inhTop = imageLoad(rdGridPrevious, top).w;
    float inhBottom = imageLoad(rdGridPrevious, bottom).w;
    float inhLeft = imageLoad(rdGridPrevious, left).w;
    float inhRight = imageLoad(rdGridPrevious, right).w;

    float actNew = act + u_timeFactor * u_deltaTime * ( (u_actDiffusivity/(u_deltaSpace*u_deltaSpace)) * (actRight + actLeft + actTop + actBottom - 4*act) + u_actCoupling*((act*act / inh) - act) + u_actBaseProduction);

    float inhNew = inh + u_timeFactor * u_deltaTime * ( (u_inhDiffusivity/(u_deltaSpace*u_deltaSpace)) * (inhRight + inhLeft + inhTop + inhBottom - 4*inh) + u_inhCoupling * (act*act - inh) + u_inhBaseProduction );

    newElement.z = actNew, 0.0, 100.0;
    newElement.w = inhNew, 0.0, 100.0;
    imageStore(rdGrid, index, newElement);
}
