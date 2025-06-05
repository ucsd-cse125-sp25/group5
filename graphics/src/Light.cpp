#include "Light.h"
#include <iostream>
#include <vector>

#define MAX_LIGHTS 150
#define BINDING_POINT 0

void Lights::init() {
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT, SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    std::srand(static_cast<unsigned>(std::time(0)));


    for (int i = 0; i < MAX_LIGHTS; i++) {
        Light light;

        float x = -75.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 150.0f));
        float y = 0.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100.0f));
        float z = -75.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 150.0f));
        light.position = glm::vec4(x, y, z, 1.0f);

        // Generate random bright color components between 0.5 and 1.0
        float r = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.5f));
        float g = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.5f));
        float b = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.5f));

        light.ambient = glm::vec4(0.1f * r, 0.1f * g, 0.1f * b, 1.0f);  // scaled down for ambient
        light.diffuse = glm::vec4(0.8f * r, 0.8f * g, 0.8f * b, 1.0f);
        light.specular = glm::vec4(1.0f * r, 1.0f * g, 1.0f * b, 1.0f);

        light.constant = 1.0f;
        light.linear = 0.07f;
        light.quadratic = 0.017f;

        addLight(light);
    }

    //// Initialize example lights
    //Light light1;
    //light1.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // Light 1 position (0, 0, 0)
    //light1.ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    //light1.diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    //light1.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //light1.constant = 1.0f;
    //light1.linear = 0.09f;
    //light1.quadratic = 0.032f;
    //addLight(light1);

    //Light light2;
    //light2.position = glm::vec4(-2.0f, 3.0f, 1.0f, 1.0f);  // Light 2 position (-2, 3, 1)
    //light2.ambient = glm::vec4(0.1f, 0.0f, 0.0f, 1.0f);  // Red ambient light
    //light2.diffuse = glm::vec4(0.7f, 0.0f, 0.0f, 1.0f);  // Red diffuse light
    //light2.specular = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
    //light2.constant = 1.0f;
    //light2.linear = 0.09f;
    //light2.quadratic = 0.032f;
    //addLight(light2);

    //Light light3;
    //light3.position = glm::vec4(3.0f, -2.0f, 1.5f, 1.0f);  // Light 3 position (3, -2, 1.5)
    //light3.ambient = glm::vec4(0.0f, 0.0f, 0.1f, 1.0f);  // Blue ambient light
    //light3.diffuse = glm::vec4(0.0f, 0.0f, 0.7f, 1.0f);  // Blue diffuse light
    //light3.specular = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
    //light3.constant = 1.0f;
    //light3.linear = 0.09f;
    //light3.quadratic = 0.032f;
    //addLight(light3);

    dirLight.color = glm::vec3(0.7, 0.75, 0.68)/3.0f;
    dirLight.direction = glm::normalize(glm::vec3(-1.0, 0.44, -0.26));
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

DirectionalLight* Lights::getDirLight() {
    return &dirLight;
}