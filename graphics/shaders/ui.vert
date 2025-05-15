#version 430 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 color;

out vec3 FragColor;
out vec2 TexCoord;
uniform mat4 projection;
uniform mat4 model = mat4(1.0f);

void main() {
    gl_Position = projection * model * vec4(position, 0.0, 1.0);
    TexCoord = uv;
    FragColor = color;
}