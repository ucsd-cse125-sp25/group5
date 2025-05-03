#version 430 core

in vec2 TexCoord;
in vec3 FragColor;

out vec4 fragColor;

uniform float manaPercent;
uniform bool isMana;
uniform float time;
uniform sampler2D texture1;

void main() {
    float waveAmp = 0.03;
    float waveFreq = 12.0;
    float waveSpeed = 0.75;

    float wave = sin(TexCoord.x * waveFreq + time * waveSpeed) * waveAmp;

    if (TexCoord.y >= (manaPercent + wave) && isMana) {
        discard;
    }
    vec4 texColor = texture(texture1, TexCoord);
    fragColor = texColor * vec4(FragColor, 1.0);
}