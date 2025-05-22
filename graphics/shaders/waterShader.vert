#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 aTexCoord;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. 
out vec3 FragPos;
out vec3 FragNormal;
out vec2 TexCoord;

// Uniform variables
uniform mat4 viewProj;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform float time;
out vec4 FragPosLightSpace;

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float f(float inpt){
    return exp(sin(inpt) - 1) - 0.5;
}

vec4 FBM(vec3 pos){ //Returns float4 of height and normal
    float y = 0;
    float dx = 0;
    float dz = 0;

    float amp = 0.16;
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

        //partial derivative
        float d = amp * freq * exp(sin(inpt) - 1.0) * cos(inpt);
        dx += dir.x * d;
        dz += dir.y * d;

        freq *= 1.07;
        amp *= 0.95;
    }

    vec3 normal = normalize(vec3(-dx, 1.0, -dz));
    vec4 ret = vec4(normal.xyz, y);
    return ret;

}

void main()
{
    vec3 adjpos = vec3(model * vec4(position, 1.0));
    vec4 ret = FBM(adjpos);
    //FragPos = vec3(adjpos.x, adjpos.y + sin(time), adjpos.z);
    FragPos = vec3(adjpos.x, adjpos.y + ret.w, adjpos.z);
    FragNormal = ret.xyz;

    //FragPos = adjpos;
    //FragNormal = mat3(transpose(inverse(model))) * normal; //might pass the transpose inverse through uniform instead

    TexCoord = aTexCoord;

    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = viewProj * vec4(FragPos, 1.0);
}