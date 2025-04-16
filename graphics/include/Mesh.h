#pragma once

#include <vector>

#include "core.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh {
private:
    GLuint VAO, texture;
    GLuint VBO_positions, VBO_normals, VBO_UVs, EBO;

    //Mesh info
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<glm::vec2> uvs;
    bool tex;

public:

    glm::vec3 color;
    glm::mat4 model;

    void create(aiMesh* mMesh, aiMaterial* mMaterial, glm::mat4 model);
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void update(glm::mat4 newModel);
};