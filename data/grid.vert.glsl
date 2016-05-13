#version 430 core

layout(location = 0) uniform mat4 u_PV;
layout(location = 1) uniform mat4 u_M;
layout(location = 2) in vec4 a_vertex;

subroutine float getScalar(vec4 vertex);
subroutine uniform getScalar u_scalar;

out float v_scalar;

subroutine(getScalar)
float activator(vec4 vertex) {
    return vertex.z;
}

subroutine(getScalar)
float inhibitor(vec4 vertex) {
    return vertex.w;
}

void main() {
    v_scalar = u_scalar(a_vertex);
    gl_Position = u_PV * u_M * vec4( a_vertex.xy, -1.0, 1.0 );
}
