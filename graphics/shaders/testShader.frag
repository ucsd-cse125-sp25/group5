#version 430 core
out vec4 fragColor;

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoord;

uniform vec3 AmbientColor = vec3(0.2);
uniform vec3 viewPos;
uniform vec3 DiffuseColor;
uniform sampler2D tex;
uniform int istex = 0;

struct Light {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float padding;
};

layout(std430, binding = 0) buffer LightBuffer {
	Light lights[];
};

void main() 
{
    //fragColor = vec4(sqrt(result), 1.0);

    //Normalized light direction from light to fragment
    vec3 norm = normalize(FragNormal);

    //Assuming only light for now
    Light light = lights[0];

    //Calculate the light direction
    vec3 lightDir = normalize(vec3(light.position) - FragPos);

    //Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    //Texture color, fallback to diffusecolor
    vec3 texColor = istex == 1 ? vec3(texture(tex, TexCoord)) : DiffuseColor;

    vec3 ambient = AmbientColor;
    vec3 diffuse = light.diffuse.rgb * diff * texColor;

    //Combine results
    vec3 result = ambient + diffuse;

    fragColor = vec4(result, 1.0);
}