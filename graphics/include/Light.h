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

class Lights {
public:
	void init();
	void update();
	void bind();
    void addLight(Light &l);
private:
	GLuint SSBO;
    std::vector<Light> lights;
};