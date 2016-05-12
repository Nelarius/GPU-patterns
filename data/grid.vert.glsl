#version 430 core

layout(location = 0) uniform mat4 u_PV;
layout(location = 1) uniform mat4 u_M;

layout(location = 2) in vec4 a_vertex;
out float v_scalar;

void main() {
    v_scalar = a_vertex.z;
    gl_Position = u_PV * u_M * vec4( a_vertex.xy, -1.0, 1.0 );
}
