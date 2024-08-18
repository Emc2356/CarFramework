#version 450

layout(set=0, binding=0) uniform UniformBufferObject {
    mat4 uProj;
};

layout(location=0) in vec2 iPos;
layout(location=1) in vec3 iColor;
layout(location=2) in vec2 iUv;

layout(location=0) out vec3 oColor;
layout(location=1) out vec2 oUv;

void main() {
    gl_Position = uProj * vec4(iPos, 0.0, 1.0);
    oColor = iColor;
    oUv = iUv;
}
