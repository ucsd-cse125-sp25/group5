#pragma once

#include <vector>

#include "core.h"
#include "glm/gtx/string_cast.hpp"

class Cube {
private:
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::mat4 model;
    glm::vec3 color;

    // Cube Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

public:
    Cube(glm::vec3 cubeMin = glm::vec3(-1, -1, -1), glm::vec3 cubeMax = glm::vec3(1, 1, 1));
    ~Cube();

    void create(glm::vec3 cubeMin, glm::vec3 cubeMax, glm::mat4 new_model);
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void update(glm::vec3 cubeMin, glm::vec3 cubeMax, glm::mat4 new_model);

    void spin(float deg);
    void setModel(glm::mat4 model) { this->model = model; }
};