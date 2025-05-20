#version 430 core

in vec2 TexCoord;
in vec3 FragColor;
in vec2 posOut;

out vec4 fragColor;

uniform sampler2D texture1;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    vec4 shadow = texture(texture1, TexCoord + vec2(-0.07, 0.06));
    //transparent background
    if (texColor.a == 0.0 && shadow.a == 0.0) {
        fragColor = vec4(1.0,1.0,1.0, 0.2);
        return;
    }
    //is a shadow
    else if(texColor.a == 0.0){
        fragColor = vec4(0.0,0.0,0.0, 0.9);
    }
    else{
        fragColor = vec4(1.0-TexCoord.x * TexCoord.y, 0.5, TexCoord.y/2.0 + 0.5, 1.0);//texColor * vec4(FragColor, 1.0);
    }
}