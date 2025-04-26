#pragma once

#include <vector>

#include "core.h"

class Triangle {
private:
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    float cr = 0.001;
    float cg = 0.001;
    float cb = 0.001;

    glm::mat4 model;

    // Triangle Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

public:
    Triangle();
    Triangle(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> triangles);
    ~Triangle();

    glm::vec3 color;

    void create(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> triangles, glm::mat4 new_model);
    void draw(GLuint shader, bool shadow);
    void update(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> triangles, glm::mat4 new_model);

    void spin(float deg);
};