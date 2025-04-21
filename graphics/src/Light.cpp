#include "Light.h"

#define MAX_LIGHTS 50
#define BINDING_POINT 0

void Lights::init() {
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT, SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Lights::update() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Light) * lights.size(), lights.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void Lights::bind() {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT, SSBO);
}