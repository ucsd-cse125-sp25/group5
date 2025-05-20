#version 330 core

in vec2 TexCoord;
in vec3 FragColor;


out vec4 fragColor;

uniform sampler2D texture1;
uniform float percentage;
uniform bool isBranch;
uniform float decay;

void main() {

    float finalAlpha = 1.0;
    if (TexCoord.x > percentage && isBranch) {
        discard;
    }
    vec4 texColor = texture(texture1, TexCoord);

    if (!isBranch) {
        finalAlpha = (1.0 - decay) * texColor.a;
    }
    fragColor = texColor * vec4(FragColor, finalAlpha);
}