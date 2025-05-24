#version 430 core
out vec4 fragColor;

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

uniform vec3 AmbientColor = vec3(0.5);
uniform int numLights; 
uniform vec3 viewPos;
uniform vec3 DiffuseColor;
uniform sampler2D tex;
uniform sampler2D shadowMap;
uniform int istex = 0;
uniform bool useShadow;
uniform vec3 dirLightDir;
uniform vec3 dirLightColor;
uniform vec3 dirLightSpec;

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

float bias = 0.005;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    //float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    if(projCoords.z > 1.0)
        return 0.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 PointLightCalc(Light light, vec3 norm, vec3 FragPos, vec3 texColor, vec3 viewDir, float shadow) {
    vec3 lightDir = normalize(vec3(light.position) - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    //Specular Shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

    //Attenuation
    float distance = length(vec3(light.position) - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 diffuse = light.diffuse.rgb * diff * texColor * attenuation;
    vec3 specular = light.specular.rgb * spec * attenuation;

    return (1.0 - shadow) * (diffuse + specular);
}

vec3 DirLightCalc(vec3 norm, vec3 viewDir, vec3 texColor, float shadow) {
    vec3 dirLightFinal = dirLightColor;
    vec3 dirLightDirNorm = normalize(dirLightDir);
    float diff = max(dot(norm, dirLightDirNorm), 0.0);

    //Specular shading for directional light
    vec3 reflectDir = reflect(-dirLightDirNorm, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);

    //Combine results for directional light (with shadow applied)
    return (1.0 - shadow) * (dirLightFinal * diff + dirLightSpec * spec);
}

void main() 
{
    vec3 norm = normalize(FragNormal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 ambient = AmbientColor;
    vec3 texColor = istex == 1 ? vec3(texture(tex, TexCoord)) : DiffuseColor;

    vec3 result = ambient * texColor;
    float shadow = useShadow ? ShadowCalculation(FragPosLightSpace) : 0.0;

    //result += DirLightCalc(norm, viewDir, texColor, shadow);

    //loop through all the point lights
    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];
        result += PointLightCalc(light, norm, FragPos, texColor, viewDir, shadow);
    }

    fragColor = vec4(result, 1.0);
}