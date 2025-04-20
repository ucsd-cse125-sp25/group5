#pragma once

#include <vector>

#include "core.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Mesh.h>

class Object {
private:

    glm::mat4 model; //also model matrix for all children meshes
    GLuint shader;

    std::vector<Mesh*> meshes;

public:

    void create(char* filename, glm::mat4 model, int shaderIndex);
    void draw(const glm::mat4& viewProjMtx);
    void update(glm::mat4 new_model);
};