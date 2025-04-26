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
out vec4 FragPosLightSpace;

void main()
{
    FragPos = vec3(model * vec4(position, 1.0));
    FragNormal = mat3(transpose(inverse(model))) * normal; //might pass the transpose inverse through uniform instead
    TexCoord = aTexCoord;

    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = viewProj * vec4(FragPos, 1.0);
}