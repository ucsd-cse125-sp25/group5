#pragma once

#include <vector>
#include <string>
// TODO: #include <stb_image.h> import this later...
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "core.h"


class Skybox {
private:
	GLuint skyboxVAO, skyboxVBO, cubemapTexture;
	std::vector<std::string> faces; //texture paths

	GLuint shaderProgram;


public:
	void initSkybox();
	GLuint loadCubemap(const std::vector<std::string> faces);
	void draw(Camera* cam);
};