#version 330 core

in vec2 TexCoord;
in vec3 FragColor;

out vec4 fragColor;

uniform sampler2D texture1;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    fragColor = texColor * vec4(FragColor, 1.0);
}