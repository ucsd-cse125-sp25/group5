#pragma once

#ifndef SKIN_DEF
#define SKIN_DEF


#include "core.h"
#include "glm/gtx/euler_angles.hpp"
#include "Joint.h"
#include "SkinWeights.h"
#include "Triangle.h"
#include <vector>

class Skin {
public:
    Skin(Skeleton* skinsskel);
    Skeleton* skely;
	Triangle* tri;
    std::vector<SkinWeights*> sw;
    std::vector<glm::mat4> bindMatrices;
    bool doSkin = false;
    void doSkinning();
    bool Load(const char* file);
    void update(); //smooth skin algorithm
    void draw(GLuint shader, bool shadow);

private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> triangles;
	std::vector<glm::mat4> bindings;
    glm::mat4 oneglm;
};


#endif // !SKIN_DEF