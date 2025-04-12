#include "Object.h"
#include <iostream>

void Object::create(char* filename) {
    model = glm::mat4(1.0f);
    color = glm::vec3(0.1f, 0.95f, 0.9f);

    std::cout << "entered create" << std::endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    std::cout << "Number of Meshes: " << scene->mNumMeshes << std::endl;
    int count = 0;
    for (int i = 0; i < scene->mNumMeshes; i++) {

        for (int j = 0; j < scene->mMeshes[i]->mNumFaces; j++) {

            indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[0] + count);
            indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[1] + count);
            indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[2] + count);
        }

        for (int j = 0; j < scene->mMeshes[i]->mNumVertices; j++) {
            glm::vec3 vert(scene->mMeshes[i]->mVertices[j].x, scene->mMeshes[i]->mVertices[j].y, scene->mMeshes[i]->mVertices[j].z);
            positions.push_back(vert);
            glm::vec3 normal(scene->mMeshes[i]->mNormals[j].x, scene->mMeshes[i]->mNormals[j].y, scene->mMeshes[i]->mNormals[j].z);
            normals.push_back(normal);
        }
        //std::cout << "Number of faces for mesh: " << i << " " << scene->mMeshes[i]->mNumFaces << std::endl;
        //std::cout << "Number of vertices for mesh: " << i << " " << scene->mMeshes[i]->mNumVertices << std::endl;
        //std::cout << "Number of normals for mesh: " << i <<  " " << normals.size() << std::endl;
        count += scene->mMeshes[i]->mNumVertices;
    }
    //for (int i = 0; i < scene->mNumMeshes; i++) {
    //    std::cout << "printing all faces in mesh " << i << std::endl;
    //    for (int j = 0; j < scene->mMeshes[i]->mNumFaces; j++) {
    //        std::cout << scene->mMeshes[i]->mFaces[j].mIndices[0] << " " << scene->mMeshes[i]->mFaces[j].mIndices[1] << " " << scene->mMeshes[i]->mFaces[j].mIndices[2] << std::endl;
    //    } 
    //}

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
void Object::draw(const glm::mat4& viewProjMtx, GLuint shader) {
    glUseProgram(shader);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
    glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

    // Bind the VAO
    glBindVertexArray(VAO);

    // draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}
void Object::update(glm::mat4 new_model) {
    model = new_model;
}