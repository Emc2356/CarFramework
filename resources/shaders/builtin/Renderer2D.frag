#version 450 core

layout(location=0) in vec2 iSourceUV;
layout(location=1) in vec3 iTint;
layout(location=2) in flat uint iTextureID;

layout(location=0) out vec4 oColor;

layout(set=0, binding=0) uniform sampler2D uTexture0;
layout(set=0, binding=1) uniform sampler2D uTexture1;
layout(set=0, binding=2) uniform sampler2D uTexture2;
layout(set=0, binding=3) uniform sampler2D uTexture3;
layout(set=0, binding=4) uniform sampler2D uTexture4;
layout(set=0, binding=5) uniform sampler2D uTexture5;
layout(set=0, binding=6) uniform sampler2D uTexture6;
layout(set=0, binding=7) uniform sampler2D uTexture7;

void main() {
    switch (iTextureID) {
    case 0: oColor = texture(uTexture0, iSourceUV); break;
    case 1: oColor = texture(uTexture1, iSourceUV); break;
    case 2: oColor = texture(uTexture2, iSourceUV); break;
    case 3: oColor = texture(uTexture3, iSourceUV); break;
    case 4: oColor = texture(uTexture4, iSourceUV); break;
    case 5: oColor = texture(uTexture5, iSourceUV); break;
    case 6: oColor = texture(uTexture6, iSourceUV); break;
    case 7: oColor = texture(uTexture7, iSourceUV); break;
    case 8: oColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); break;
    }
    oColor.rgb *= iTint;
}
