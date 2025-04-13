#include "Object.h"
#include <iostream>

void Object::create(char* filename, glm::mat4 model) {
    this->model = model;

    std::cout << "entered create" << std::endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    //std::cout << "Number of Meshes: " << scene->mNumMeshes << std::endl;
    //for (int i = 0; i < scene->mNumMeshes; i++) {
    //    aiColor4D diffuse;
    //    aiGetMaterialColor(scene->mMaterials[scene->mMeshes[i]->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &diffuse);
    //    std::cout << diffuse.r << " " << diffuse.g << " " << diffuse.b << std::endl;
    //}
    //int count = 0;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        Mesh* mesh = new Mesh();
        mesh->create(scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex], model);
        meshes.push_back(mesh);
    }
}

void Object::draw(const glm::mat4& viewProjMtx, GLuint shader) {
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i]->draw(viewProjMtx, shader);
    }
}
void Object::update(glm::mat4 new_model) {
    model = new_model;
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i]->update(model);
    }
}