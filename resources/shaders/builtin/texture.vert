#version 450 core

layout(location=0) in vec2 iPos;
layout(location=1) in vec2 iSourceUV;
layout(location=2) in float iTextureID;

layout(location=0) out vec2 oSourceUV;
layout(location=1) out flat uint oTextureID;

layout(std140, binding=0) uniform CarData {
    mat4 uProj;
};

void main() {
    gl_Position = uProj * vec4(iPos, 1.0f, 1.0f);
    oSourceUV = iSourceUV; 
    oTextureID = uint(iTextureID);
}