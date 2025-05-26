#version 430 core

in vec2 TexCoord;
in vec3 FragColor;

out vec4 fragColor;

uniform sampler2D texture1;
uniform bool isLow;
uniform bool isAlive;


void main() {
    vec4 texColor = texture(texture1, TexCoord);

    vec4 result;
    float alpha = texColor.a;
    if (isAlive && isLow) {
        result = vec4(1.0, 0.0, 0.0, alpha);
    } else if (!isAlive){
        result = texColor * vec4(FragColor, 1.0);
    } else {
        result = vec4(0.0, 0.0, 0.0, 0.0);
    }

    fragColor = result;
}