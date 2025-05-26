#version 430 core

in vec2 TexCoord;
in vec3 FragColor;

out vec4 fragColor;

uniform sampler2D texture1;
uniform float time;
uniform bool isLow;
uniform bool isAlive;


void main() {
    vec4 texColor = texture(texture1, TexCoord);

    vec4 result;
    float alpha = texColor.a;
    if (isAlive && isLow) {
        float pulse = 0.5 + 0.5 * sin(time * 3.0);
        alpha *= pulse;
        alpha = clamp(alpha, 0.0, 0.25);
        result = vec4(1.0, 0.0, 0.0, alpha);
    } else if (!isAlive){
        result = texColor * vec4(FragColor, 1.0);
    } else {
        result = vec4(0.0, 0.0, 0.0, 0.0);
    }

    fragColor = result;
}