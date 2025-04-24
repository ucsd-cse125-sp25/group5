#version 430 core
out vec4 fragColor;

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoord;

uniform vec3 AmbientColor = vec3(0.5);
uniform int numLights; 
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
    vec3 norm = normalize(FragNormal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 ambient = AmbientColor;
    vec3 texColor = istex == 1 ? vec3(texture(tex, TexCoord)) : DiffuseColor;

    vec3 result = ambient * texColor;

    //loop through all the point lights
    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];
        vec3 lightDir = normalize(vec3(light.position) - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        //Specular shading
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

        //Attenuation
        float distance = length(vec3(light.position) - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

        vec3 diffuse = light.diffuse.rgb * diff * texColor * attenuation;
        vec3 specular = light.specular.rgb * spec * attenuation;

        //Combine results
        result += diffuse + specular;
    }


    fragColor = vec4(result, 1.0);
}