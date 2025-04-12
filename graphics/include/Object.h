#pragma once

#include <vector>

#include "core.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Object {
private:
    GLuint VAO, texture;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::mat4 model;

    // Cube Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

public:

    glm::vec3 color;

    void create(char* filename);
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void update(glm::mat4 new_model);
};