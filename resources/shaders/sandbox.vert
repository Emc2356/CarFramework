#version 450

layout(set=0, binding=0) uniform UniformBufferObject {
    mat4 uModel;
    mat4 uView;
    mat4 uProj;
};

layout(location=0) in vec2 iPos;
layout(location=1) in vec3 iColor;

layout(location=0) out vec3 oColor;

void main() {
    gl_Position = uProj * uView * uModel * vec4(iPos, 0.0, 1.0);
    oColor = iColor;
}
