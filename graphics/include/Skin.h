#pragma once

#ifndef SKIN_DEF
#define SKIN_DEF


#include "core.h"
#include "glm/gtx/euler_angles.hpp"
#include "SkinWeights.h"
#include "Triangle.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

class Skeleton;

class Skin {
public:
    Skin(Skeleton* skinsskel);
    void Preload(int capacity);
    Skeleton* skely;
	Triangle* tri;
    std::vector<SkinWeights*> sw;
    std::vector<glm::mat4> bindMatrices;
    bool doSkin = false;
    void doSkinning();
    bool Load(aiMesh* mMesh, aiMaterial* mMaterial);
    bool Load(const char* file);
    void update(); //smooth skin algorithm
    void draw(GLuint shader, bool shadow);

private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> triangles;
    std::vector<glm::vec2> uvs;
	std::vector<glm::mat4> bindings;
    glm::mat4 oneglm;
};


#endif // !SKIN_DEF