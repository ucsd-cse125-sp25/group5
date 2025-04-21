#pragma once

#include "core.h"
#include <vector>

struct Light {
    glm::vec4 position; //w = 1.0 (point) 0.0 (directional)
    glm::vec4 color;    //rgb + a
    float intensity;    //strength of light
    float radius;       //point light attenuation
    int type;           //point = 0, directional = 1
    float padding[5];   //16-byte aligned + multiple of 64
};

class Lights {
public:
	std::vector<Light> lights;
	void init();
	void update();
	void bind();
private:
	GLuint SSBO;
};