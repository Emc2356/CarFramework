#version 450 core

layout(location=0) in vec3 iPos;
layout(location=1) in vec2 iUv;

layout(location=0) out vec2 oUv;

layout(std140, binding=0) uniform CarData {
    mat4 uMVP;
};

void main() {
    gl_Position = uMVP * vec4(iPos, 1.0f);
    oUv = iUv;
}