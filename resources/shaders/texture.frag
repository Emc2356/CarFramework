#version 450 core

layout(location=0) in vec2 iUv;

layout(location=0) out vec4 oColor;

layout(binding=5) uniform sampler2D uTexture;


void main() {
    oColor = texture(uTexture, iUv);
}