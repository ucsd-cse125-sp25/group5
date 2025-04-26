#include "Mesh.h"
#include <iostream>
#include <stb_image.h>

std::string getLastPathPart(const std::string& path);

void Mesh::create(aiMesh* mMesh, aiMaterial* mMaterial, glm::mat4 model) {
    tex = false;
    this->model = model;

    indices.reserve(mMesh->mNumFaces * 3);
    for (int j = 0; j < mMesh->mNumFaces; j++) {

        indices.push_back(mMesh->mFaces[j].mIndices[0]);
        indices.push_back(mMesh->mFaces[j].mIndices[1]);
        indices.push_back(mMesh->mFaces[j].mIndices[2]);
    }

    positions.reserve(mMesh->mNumVertices);
    normals.reserve(mMesh->mNumVertices);
    for (int j = 0; j < mMesh->mNumVertices; j++) {
        glm::vec3 vert(mMesh->mVertices[j].x, mMesh->mVertices[j].y, mMesh->mVertices[j].z);
        positions.push_back(vert);
        glm::vec3 normal(mMesh->mNormals[j].x, mMesh->mNormals[j].y, mMesh->mNormals[j].z);
        normals.push_back(normal);
    }

    if (mMesh->HasTextureCoords(0)) {
        uvs.reserve(mMesh->mNumVertices);
        for(int t = 0; t < mMesh->mNumVertices; t++){
            float x = mMesh->mTextureCoords[0][t].x;
            float y = mMesh->mTextureCoords[0][t].y;
            glm::vec2 uv(x, y);
            uvs.push_back(uv);
        }
    }

    aiColor4D diffuse;
    aiGetMaterialColor(mMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
    color.x = diffuse.r;
    color.y = diffuse.g;
    color.z = diffuse.b;

    aiString texPath;
    if (mMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        const char* path = texPath.C_Str();
        std::string filename = getLastPathPart(std::string(path));
        std::string source = PROJECT_SOURCE_DIR;
        std::string  middle = "/assets/textures/";
        source = source + middle + filename;

        int width, height, nrChannels;
        unsigned char* data = stbi_load(source.c_str(), &width, &height, &nrChannels, 0);

        if (data)
        {
            tex = true;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }

        stbi_image_free(data);

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

void Mesh::draw(GLuint shader, bool shadow) {
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

void Mesh::update(glm::mat4 newModel) {
    model = newModel;
}

std::string getLastPathPart(const std::string& path) {
    size_t end = path.find_last_not_of('/');
    if (end == std::string::npos) {
        return ""; // The path only contains slashes
    }

    size_t start = path.find_last_of('/', end);
    if (start == std::string::npos) {
        return path.substr(0, end + 1); // No slash found, return whole string
    }

    return path.substr(start + 1, end - start);
}