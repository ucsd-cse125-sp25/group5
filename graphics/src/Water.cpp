#include "Water.h"
#include <iostream>
#include <stb_image.h>

std::string getLastPathPart(const std::string& path);

void Water::create(int vertsX, int vertsZ, float spacing, float y) {
    tex = false;
    model = glm::mat4(1);
    color = glm::vec3(0.2f, 0.2f, 0.8f);
    positions.clear();
    normals.clear();
    uvs.clear();
    indices.clear();

    for (int z = 0; z < vertsZ; ++z) {
        for (int x = 0; x < vertsX; ++x) {
            positions.emplace_back(x * spacing, y, z * spacing);
            normals.emplace_back(0.0f, 1.0f, 0.0f);
            uvs.emplace_back((float)x / (vertsX - 1), (float)z / (vertsZ - 1));
        }
    }

    for (int z = 0; z < vertsZ - 1; ++z) {
        for (int x = 0; x < vertsX - 1; ++x) {
            int topLeft = z * vertsX + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * vertsX + x;
            int bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }


     // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);
    glGenBuffers(1, &VBO_UVs);

    // Bind to the VAO.
    glBindVertexArray(VAO);

    // Bind to the first VBO - We will use it to store the vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Bind to the second VBO - We will use it to store the normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Bind to the third VBO - We will use it to store the UVs
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UVs);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    // Generate EBO, bind the EBO to the bound VAO and send the data
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  
}

void Water::draw(GLuint shader, bool shadow) {
    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);

    if (!shadow) {
        glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);
        int istex = tex;
        glUniform1i(glGetUniformLocation(shader, "istex"), istex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shader, "tex"), 0);
    }

    // Bind the VAO
    glBindVertexArray(VAO);
    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO and shader program
    glBindVertexArray(0);
    if (!shadow) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
}

void Water::update(glm::mat4 newModel) {
    model = newModel;
}