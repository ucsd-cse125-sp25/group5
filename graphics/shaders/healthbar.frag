#version 330 core
uniform float healthPercent;
uniform bool health;

in vec2 TexCoord;
in vec3 FragColor;


out vec4 fragColor;

uniform sampler2D texture1;

void main() {

    if (TexCoord.x >= healthPercent && health) {
        discard;
    }
    vec4 texColor = texture(texture1, TexCoord);
    fragColor = texColor * vec4(FragColor, 1.0);
}