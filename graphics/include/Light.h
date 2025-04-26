#pragma once

#include "core.h"
#include <vector>

//Only point light, we can just pass in 1 directional light
struct Light {
    glm::vec4 position; //w = 1.0 (point)
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float padding;
};

struct DirectionalLight {
    glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::mat4 getLightSpaceMatrix() const {
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 30.0f);
        glm::mat4 lightView = glm::lookAt(-direction * 10.0f, glm::vec3(0.0f), glm::vec3(0, 1, 0));
        return lightProjection * lightView;
    }
};

class Lights {
public:
	void init();
	void update();
	void bind();
    void addLight(Light &l);
    int numLights();
    glm::mat4 getDirLightMat() const { return dirLight.getLightSpaceMatrix(); };
    glm::vec3 getDirLightDir() const { return dirLight.direction; };
private:
	GLuint SSBO;
    std::vector<Light> lights;
    DirectionalLight dirLight; //probably only one which will resemble the moon
};