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
uniform float time;
uniform float waterLevel;
uniform float fogConstant;
uniform float fogConstantW;
uniform vec3 fogColor;
uniform vec3 fogColorW;

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

// Simplex 2D noise
//
vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float f(float inpt){
    return exp(sin(inpt) - 1) - 0.5;
}

vec3 FBM(vec3 pos){ //Returns float3 of normal
    float y = 0;
    float dx = 0;
    float dz = 0;

    float amp = 0.03;
    float freq = 1.5;

    float pc = 1.6;

    for(int i = 0; i < 10; i++){
        float angle = 2.0 * 3.1415 * random(vec2(i, 25.0));
        vec2 dir = vec2(cos(angle), sin(angle));
        float v = dot(dir, vec2(pos.x, pos.z));
        float phase = random(vec2(i, 99.99)) * 6.2831;
        float inpt = v * freq + time * freq * pc + phase;
        float height = amp * f(inpt);
        y += height;

        //partial derivative
        float d = amp * freq * exp(sin(inpt) - 1.0) * cos(inpt);
        dx += dir.x * d;
        dz += dir.y * d;

        freq *= 1.07;
        amp *= 0.95;
    }

    vec3 normal = normalize(vec3(-dx, 1.0, -dz));
    return normal;

}

float FBMVertex(vec3 pos){ //return y pos of water at this x and z coord
    float y = 0;
    float dx = 0;
    float dz = 0;

    float amp = 0.2;
    float freq = 0.4;

    float pc = 1.6;

    for(int i = 0; i < 15; i++){
        float angle = 2.0 * 3.1415 * random(vec2(i, 25.0));
        vec2 dir = vec2(cos(angle), sin(angle));
        float v = dot(dir, vec2(pos.x, pos.z));
        float phase = random(vec2(i, 99.99)) * 6.2831;
        float inpt = v * freq + time * freq * pc + phase;
        float height = amp * f(inpt);
        y += height;

        freq *= 1.07;
        amp *= 0.95;
    }

    return y;

}

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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 200.0);

    //Combine results for directional light (with shadow applied)
    return (1.0 - shadow) * (dirLightFinal * diff + dirLightSpec * spec);
}



void main() 
{
    vec3 norm = mix(normalize(FragNormal), FBM(FragPos), 0.6);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 ambient = AmbientColor;
    vec3 texColor = istex == 1 ? vec3(texture(tex, TexCoord)) : DiffuseColor;

    float fogWeightW = (1.0/exp(fogConstantW*length(viewPos - FragPos)));
    float fogWeight = (1.0/exp(fogConstant*length(viewPos - FragPos)));

    vec3 color1 = vec3(0.13, 0.4, 0.78);
    vec3 color2 = vec3(0.18, 0.57, 0.75);

    float rand = 0.5*sin(time + snoise(FragPos.xz/20.0) * 6.28) + 0.5;
    rand = (rand < 0.5) ? -(pow(abs(0.5 - rand) * 2.0, 2.0)/2.0) + 0.5 : (pow(abs(0.5 - rand) * 2.0, 2.0)/2.0) + 0.5; 
    float randv2 = (floor(rand * 5.99 + 0.3))/6.0;
    texColor = randv2 >= 1 ? vec3(0.85, 0.85, 0.85) : mix(color1, color2, randv2);

    // float rand = 0.5*sin(time + snoise(FragPos.xz/20.0) * 6.28) + 0.5;
    // rand = (rand < 0.5) ? -(pow(abs(0.5 - rand) * 2.0, 2.0)/2.0) + 0.5 : (pow(abs(0.5 - rand) * 2.0, 2.0)/2.0) + 0.5; 
    // float randv2 = (floor(rand * 8.99))/8.0;
    // if(randv2 >= 0.49 && randv2 <= 0.51){
    //     texColor = vec3(0.85, 0.85, 0.85);
    // }
    // else{
    //     texColor = randv2 > 0.5 ? mix(color2, color1, 2.0 * (randv2 - 0.5)) : mix(color1, color2, randv2 * 2.0);
    // }
    

    vec3 result = ambient * texColor;
    float shadow = useShadow ? ShadowCalculation(FragPosLightSpace) : 0.0;

    result += DirLightCalc(norm, viewDir, texColor, shadow);

    //loop through all the point lights
    // for (int i = 0; i < numLights; i++) {
    //     Light light = lights[i];
    //     result += PointLightCalc(light, norm, FragPos, texColor, viewDir, shadow);
    // }

    float height = FBMVertex(viewPos) + waterLevel;
    if(viewPos.y < height){
       result = mix(fogColorW, result, clamp(fogWeightW - 0.05, 0, 1));
    }
    else{
       result = mix(fogColor, result, clamp(fogWeight - 0.02, 0, 1));
    }

    fragColor = vec4(result, 1.0);
}