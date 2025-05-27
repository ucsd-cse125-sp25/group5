#version 430 core

in vec2 TexCoord;
in vec3 FragColor;

out vec4 fragColor;

uniform sampler2D texture1;
uniform float transparency;
uniform vec3 borderColor;

void main() {
    float borderThickness = 0.02;
    vec4 texColor = texture(texture1, TexCoord);
    fragColor = texColor * vec4(FragColor, transparency);
    if(TexCoord.x < borderThickness || TexCoord.x > 1 - borderThickness || TexCoord.y < borderThickness || TexCoord.y > 1 - borderThickness){
        fragColor = vec4(borderColor, transparency);
    }
}