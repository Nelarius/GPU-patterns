#version 430 core

uniform mat4 u_PV;
uniform mat4 u_M;

layout(location = 1) in vec4 a_vertex;
out float v_scalar;

void main() {
    v_scalar = a_vertex.w;
    gl_Position = u_PV * u_M * vec4( a_vertex.xyz, 1.0 );
}
