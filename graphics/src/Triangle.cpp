#include "Triangle.h"
#include <iostream>


Triangle::Triangle() {}
Triangle::Triangle(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs, std::vector<unsigned int> triangles) {

    model = glm::mat4(1.0f);
    tex = false;
    create(positions, normals, triangles, uvs, model);
    // The color of the Triangle. Try setting it to something else!
    this->color = glm::vec3(0.8f, 0.3f, 0.05f);

}

void Triangle::create(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> triangles, std::vector<glm::vec2> uvs, glm::mat4 new_model) {
    // Model matrix.
    this->model = new_model;

    // Specify vertex positions
    this->positions = positions;
       
    // Specify normals
    this->normals = normals;

     // Specify indices
	this->indices = triangles;

    //specify uvs
    this->uvs = uvs;

   
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);
    if (tex) {
        glGenBuffers(1, &VBO_UVs);
    }
    glGenBuffers(1, &EBO);
    // Bind to the VAO.

    glBindVertexArray(VAO);
    // Generate EBO, bind the EBO to the bound VAO and send the data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

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

    if (tex) {
        // Bind to the third VBO - We will use it to store the UVs
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UVs);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    }
    // Unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Triangle::~Triangle() {
    // Delete the VBOs and the VAO.
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &VBO_normals);
    if (tex) {
        glDeleteBuffers(1, &VBO_UVs);
    }
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Triangle::draw(GLuint shader, bool shadow) {
    this->cr = this->cr + 0.0001;
    this->cg = this->cg + 0.0001;
    this->cb = this->cb + 0.0001;
    //this->color = glm::vec3(this->cr, this->cg, this->cb);

    if (this -> cr > 1.0) this -> cr = 0;
    if (this -> cg > 1.0) this -> cg = 0;
    if (this -> cb > 1.0) this -> cb = 0;

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);

    if (!shadow) {
        glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);
        glUniform1i(glGetUniformLocation(shader, "istex"), tex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        GLint texLoc = glGetUniformLocation(shader, "tex");
        glUniform1i(texLoc, 0);
    }
    // Bind the VAO
    glBindVertexArray(VAO);
    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO and shader program

    if (!shadow) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }    
    glBindVertexArray(0);
}

void Triangle::update(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> uvs, std::vector<unsigned int> triangles, glm::mat4 new_model) {
    // Update model matrix and geometry data
    this->model = new_model;
    this->positions = positions;
    this->normals = normals;
    this->uvs = uvs;
    this->indices = triangles;

    // Bind the VAO
    glBindVertexArray(VAO);

    // Update vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    // Update normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    // Conditionally update UVs
    if (tex) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UVs);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
    }

    // Now unbind GL_ARRAY_BUFFER (once, after all VBO uploads)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Update index buffer (EBO) — must remain bound to VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Unbind VAO (leave EBO bound inside it)
    glBindVertexArray(0);
}
