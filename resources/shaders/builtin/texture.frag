#version 450 core

layout(location=0) in vec2 iSourceUV;
layout(location=1) in vec3 iTint;
layout(location=2) in flat uint iTextureID;

layout(location=0) out vec4 oColor;

layout(binding=0)  uniform sampler2D uTexture0;
layout(binding=1)  uniform sampler2D uTexture1;
layout(binding=2)  uniform sampler2D uTexture2;
layout(binding=3)  uniform sampler2D uTexture3;
layout(binding=4)  uniform sampler2D uTexture4;
layout(binding=5)  uniform sampler2D uTexture5;
layout(binding=6)  uniform sampler2D uTexture6;
layout(binding=7)  uniform sampler2D uTexture7;
layout(binding=8)  uniform sampler2D uTexture8;
layout(binding=9)  uniform sampler2D uTexture9;
layout(binding=10) uniform sampler2D uTexture10;
layout(binding=11) uniform sampler2D uTexture11;
layout(binding=12) uniform sampler2D uTexture12;
layout(binding=13) uniform sampler2D uTexture13;
layout(binding=14) uniform sampler2D uTexture14;
layout(binding=15) uniform sampler2D uTexture15;

void main() {
    switch (iTextureID) {
    case 0: oColor = texture(uTexture0 , iSourceUV); break;
    case 1: oColor = texture(uTexture1 , iSourceUV); break;
    case 2: oColor = texture(uTexture2 , iSourceUV); break;
    case 3: oColor = texture(uTexture3 , iSourceUV); break;
    case 4: oColor = texture(uTexture4 , iSourceUV); break;
    case 5: oColor = texture(uTexture5 , iSourceUV); break;
    case 6: oColor = texture(uTexture6 , iSourceUV); break;
    case 7: oColor = texture(uTexture7 , iSourceUV); break;
    case 8: oColor = texture(uTexture8 , iSourceUV); break;
    case 9: oColor = texture(uTexture9 , iSourceUV); break;
    case 10: oColor = texture(uTexture10, iSourceUV); break;
    case 11: oColor = texture(uTexture11, iSourceUV); break;
    case 12: oColor = texture(uTexture12, iSourceUV); break;
    case 13: oColor = texture(uTexture13, iSourceUV); break;
    case 14: oColor = texture(uTexture14, iSourceUV); break;
    case 15: oColor = texture(uTexture15, iSourceUV); break;
    }
    oColor.rgb *= iTint;
}
