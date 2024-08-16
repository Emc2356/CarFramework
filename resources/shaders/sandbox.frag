#version 450

layout(set=1, binding=1) uniform sampler2D uTexture;

layout(location=0) in vec3 iColor;
layout(location=1) in vec2 iUv;
layout(location=0) out vec4 oColor;

void main() {
    // outColor = vec4(iColor, 1.0f);
    // outColor.rg = iUv.rg;
    oColor = texture(uTexture, iUv);
}
