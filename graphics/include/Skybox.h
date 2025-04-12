#pragma once

#include <vector>
#include <string>
// TODO: #include <stb_image.h> import this later...
#include "Camera.h"
#include "core.h"


class Skybox {
private:
	GLuint skyboxVAO, skyboxVBO, cubemapTexture;
	std::vector<std::string> faces; //texture paths
	std::vector<float> skyboxVertices;


public:
	void initSkybox();
	GLuint loadCubemap(std::vector<std::string> faces);
	void draw(Camera* cam);
};