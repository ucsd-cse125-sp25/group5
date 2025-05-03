#include "Triangle.h"
#include <iostream>


Triangle::Triangle() {}
Triangle::Triangle(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> triangles) {

    model = glm::mat4(1.0f);
    create(positions, normals, triangles, model);
    // The color of the Triangle. Try setting it to something else!
    this->color = glm::vec3(0.8f, 0.3f, 0.05f);

}

void Triangle::create(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> triangles, glm::mat4 new_model) {
    // Model matrix.
    this->model = new_model;

    // Specify vertex positions
    this->positions = positions;
       
    // Specify normals
    this->normals = normals;

     // Specify indices
	this->indices = triangles;

   
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_normals);

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

    // Generate EBO, bind the EBO to the bound VAO and send the data
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Triangle::~Triangle() {
    // Delete the VBOs and the VAO.
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &VBO_normals);
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
        glUniform1i(glGetUniformLocation(shader, "istex"), 0);
    }

    // Bind the VAO
    glBindVertexArray(VAO);
    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO and shader program
    glBindVertexArray(0);
}

void Triangle::update(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> triangles, glm::mat4 new_model) {
//    this->create(positions, normals, triangles, new_model);
    // Model matrix.
    this->model = new_model;

    // Specify vertex positions
    this->positions = positions;
       
    // Specify normals
    this->normals = normals;

     // Specify indices
	this->indices = triangles;

   
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    //glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &VBO_positions);
    //glGenBuffers(1, &VBO_normals);

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

    // Generate EBO, bind the EBO to the bound VAO and send the data
    // glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // Unbind the VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}