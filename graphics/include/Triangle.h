#pragma once

#include <vector>

#include "core.h"

class Triangle {
private:
    float cr = 0.001;
    float cg = 0.001;
    float cb = 0.001;

    glm::mat4 model;

    // Triangle Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<glm::vec2> uvs;

public:
    GLuint VAO, texture;
    GLuint VBO_positions, VBO_normals, VBO_UVs, EBO;

    bool tex;

    Triangle();
    Triangle(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs, std::vector<unsigned int> triangles);
   
    ~Triangle();

    glm::vec3 color;
  
    void create(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> triangles, std::vector<glm::vec2> uvs, glm::mat4 new_model);
    //void draw(const glm::mat4& viewProjMtx, GLuint shader); //TODO: add bool shadow and remove viewProj
    void update(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs, std::vector<unsigned int> triangles, glm::mat4 new_model);
    void draw(GLuint shader, bool shadow);
    void spin(float deg);
};