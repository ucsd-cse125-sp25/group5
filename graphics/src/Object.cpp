#include "Object.h"
#include <iostream>
#include <Scene.h>

extern Scene* scene;

void Object::create(char* filename, glm::mat4 model, int shaderIndex) {
    this->model = model;

    std::cout << "entered create" << std::endl;
	Assimp::Importer importer;
	const aiScene* iscene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);
    if (!iscene || iscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !iscene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    for (int i = 0; i < iscene->mNumMeshes; i++) {
        Mesh* mesh = new Mesh();
        mesh->create(iscene->mMeshes[i], iscene->mMaterials[iscene->mMeshes[i]->mMaterialIndex], model);
        meshes.push_back(mesh);
    }

    shader = scene->shaders[shaderIndex];
}

void Object::draw(const glm::mat4& viewProjMtx) {
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