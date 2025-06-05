#version 430 core
out vec4 fragColor;

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoord;

uniform vec3 AmbientColor = vec3(0.65, 0.65, 0.9);
uniform vec3 viewPos;
uniform vec3 DiffuseColor;
uniform sampler2D tex;
uniform int istex = 0;

void main() 
{
    vec3 norm = normalize(FragNormal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 ambient = AmbientColor;
    vec3 texColor = istex == 1 ? vec3(texture(tex, TexCoord)) : DiffuseColor;

    vec3 result = ambient * texColor;
    fragColor = vec4(result, 0.6);
}