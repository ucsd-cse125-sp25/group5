#version 430 core

in vec2 TexCoord;
in vec3 FragColor;

out vec4 fragColor;

uniform float manaPercent;
uniform bool isMana;
uniform float time;
uniform sampler2D texture1;
uniform bool selectedMana = false;

void main() {
    float waveAmp = 0.05;
    float waveFreq = 12.0;
    float waveSpeed = 0.75;

    float wave = sin(TexCoord.x * waveFreq + time * waveSpeed) * waveAmp;

    if (TexCoord.y >= (manaPercent + wave) && isMana) {
        discard;
    }
    vec4 texColor = texture(texture1, TexCoord);
    vec4 finalColor = texColor * vec4(FragColor, 1.0);

    // Apply grayscale to the final color if not selected
    if (!selectedMana) {
        float gray = dot(finalColor.rgb, vec3(0.299, 0.587, 0.114)); //grayscale
        vec3 desaturated = vec3(gray);
        finalColor.rgb = mix(desaturated, vec3(0.5), 0.3); // slightly brighten gray
        finalColor.rgb *= 0.5; // dim the result
    }

    fragColor = finalColor;
}