#version 450

layout(set=1, binding=1) uniform sampler2D uTexture;

layout(location=0) in vec3 iColor;
layout(location=1) in vec2 iUv;
layout(location=0) out vec4 oColor;

void main() {
    oColor = texture(uTexture, iUv);
    oColor.rgb *= iColor.rgb;
}
