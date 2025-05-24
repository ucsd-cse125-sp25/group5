#include "Light.h"
#include <iostream>

#define MAX_LIGHTS 50
#define BINDING_POINT 0

void Lights::init() {
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT, SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Initialize example lights
    Light light1;
    light1.position = glm::vec4(2.0f, 2.0f, 0.0f, 1.0f);  // Light 1 position (2, 2, 0)
    light1.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    light1.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    light1.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    light1.constant = 1.0f;
    light1.linear = 0.09f;
    light1.quadratic = 0.032f;
    addLight(light1);

    Light light2;
    light2.position = glm::vec4(-2.0f, 3.0f, 1.0f, 1.0f);  // Light 2 position (-2, 3, 1)
    light2.ambient = glm::vec4(0.1f, 0.0f, 0.0f, 1.0f);  // Red ambient light
    light2.diffuse = glm::vec4(0.7f, 0.0f, 0.0f, 1.0f);  // Red diffuse light
    light2.specular = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
    light2.constant = 1.0f;
    light2.linear = 0.09f;
    light2.quadratic = 0.032f;
    addLight(light2);

    Light light3;
    light3.position = glm::vec4(3.0f, -2.0f, 1.5f, 1.0f);  // Light 3 position (3, -2, 1.5)
    light3.ambient = glm::vec4(0.0f, 0.0f, 0.1f, 1.0f);  // Blue ambient light
    light3.diffuse = glm::vec4(0.0f, 0.0f, 0.7f, 1.0f);  // Blue diffuse light
    light3.specular = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
    light3.constant = 1.0f;
    light3.linear = 0.09f;
    light3.quadratic = 0.032f;
    addLight(light3);

    dirLight.color = glm::vec3(0.7, 0.75, 0.68)/3.0f;
    dirLight.direction = glm::normalize(glm::vec3(1.0, 0.4, 1.0));
    dirLight.specular = glm::vec3(0.7, 0.75, 0.68)/2.0f;
}


void Lights::update() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Light) * lights.size(), lights.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void Lights::bind() {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT, SSBO);
}

void Lights::addLight(Light &l) {
	lights.push_back(l);
}

int Lights::numLights() {
    return lights.size();
}

DirectionalLight Lights::getDirLight() {
    return dirLight;
}