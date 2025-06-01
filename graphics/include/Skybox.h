#pragma once

#include <vector>
#include <string>
#include "Shader.h"
#include "Camera.h"
#include "core.h"


class Skybox {
private:
	GLuint skyboxVAO, skyboxVBO, cubemapTexture, ct1, ct2, ct3;
	std::vector<std::string> faces; //texture paths

	GLuint shaderProgram;


public:
	void initSkybox();
	GLuint loadCubemap(const std::vector<std::string> faces);
	void draw(Camera* cam, float* model);
	void updatePhase(int phase);
};