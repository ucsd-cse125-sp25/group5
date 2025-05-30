#version 430 core

in vec2 TexCoord;
in vec3 FragColor;

out vec4 fragColor;

uniform sampler2D texture1;
uniform float transparency;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    fragColor = texColor * vec4(FragColor, transparency);
}