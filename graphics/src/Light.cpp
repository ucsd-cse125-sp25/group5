#include "Light.h"
#include "Global.h"
#include <iostream>
#include <vector>

#define MAX_LIGHTS 250
#define BINDING_POINT 0


glm::vec3 hsvToRgb(float h, float s, float v) {
    float c = v * s;
    float x = c * (1 - fabsf(fmod(h * 6.0f, 2.0f) - 1));
    float m = v - c;

    glm::vec3 rgb;

    if (h < 1.0f / 6.0f)      rgb = glm::vec3(c, x, 0);
    else if (h < 2.0f / 6.0f) rgb = glm::vec3(x, c, 0);
    else if (h < 3.0f / 6.0f) rgb = glm::vec3(0, c, x);
    else if (h < 4.0f / 6.0f) rgb = glm::vec3(0, x, c);
    else if (h < 5.0f / 6.0f) rgb = glm::vec3(x, 0, c);
    else                   rgb = glm::vec3(c, 0, x);

    return rgb + glm::vec3(m);
}

void Lights::init() {
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BINDING_POINT, SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    std::srand(static_cast<unsigned>(std::time(0)));

    for (int i = 0; i < mapObjects.size(); i++) {

        if (std::get<0>(mapObjects.at(i)).find("lantern") != std::string::npos) {
            glm::vec3 pos = std::get<1>(mapObjects.at(i));
            pos -= glm::vec3(0.0, -1.0, 0.0);
            Light light;

            light.position = glm::vec4(pos, 1.0);

            // Orange glow with slight variance
            float orangeVariance = 0.05f;
            float r = 0.82f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / orangeVariance)) - orangeVariance / 2.0f;
            float g = 0.69f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / orangeVariance)) - orangeVariance / 2.0f;
            float b = 0.3f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / orangeVariance)) - orangeVariance / 2.0f;

            // Brighter light
            light.diffuse = glm::vec4(1.0f * r, 1.0f * g, 1.0f * b, 1.0f);
            light.specular = glm::vec4(1.0f * r, 1.0f * g, 1.0f * b, 1.0f);

            // Make the light reach further (bigger radius)
            light.constant = 1.0f;
            light.linear = 0.019f;      // Lower = longer reach
            light.quadratic = 0.0011f;  // Lower = slower falloff

            addLight(light);
        }
    }


    while (lights.size() < MAX_LIGHTS) {
        Light light;
        // 46.3, -14.8, -75.0
        //-68.0, 164.5, 63.9
        //float x = -75.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 150.0f));
        //float y = 0.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100.0f));
        //float z = -75.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 150.0f));
        glm::vec3 minPos(-68.0f, -14.8f, -75.0f);
        glm::vec3 maxPos(46.3f, 164.5f, 63.9f);

        // Random value between 0 and 1
        auto rand01 = []() {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            };

        float x = minPos.x + rand01() * (maxPos.x - minPos.x);
        float y = minPos.y + rand01() * (maxPos.y - minPos.y);
        float z = minPos.z + rand01() * (maxPos.z - minPos.z);

        light.position = glm::vec4(x, y, z, 1.0f);

        light.position = glm::vec4(x, y, z, 1.0f);

        // Generate random bright color components between 0.5 and 1.0
        float h = static_cast<float>(rand()) / RAND_MAX; // hue in [0,1]
        glm::vec3 color = hsvToRgb(h, 1.0f, 1.0f);       // full saturation & brightness

        light.diffuse = glm::vec4(1.5f * color, 1.0f);    // intense diffuse
        light.specular = glm::vec4(1.0f * color, 1.0f);


        light.constant = 1.0f;
        light.linear = 0.01f;
        light.quadratic = 0.015f;

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