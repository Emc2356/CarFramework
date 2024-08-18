#version 450 core

layout(location=0) in vec2 iPos;
layout(location=1) in vec2 iSourceUV;
layout(location=2) in vec3 iTint;
layout(location=3) in uint iTextureID;

layout(location=0) out vec2 oSourceUV;
layout(location=1) out vec3 oTint;
layout(location=2) out flat uint oTextureID;

layout(std140, binding=0) uniform CarData {
    mat4 uProj;
};

void main() {
    gl_Position = uProj * vec4(iPos, 0.0f, 1.0f);
    oSourceUV = iSourceUV; 
    oTint = iTint;
    oTextureID = iTextureID;
}