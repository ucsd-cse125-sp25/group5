#include "Object.h"
#include <iostream>
#include <Scene.h>
#include "PlayerObject.h"
#include "unordered_map"
#include "Joint.h"
#include "Skin.h"
#include "Skeleton.h"
unsigned int CountNodes(const aiNode* node);

extern Scene* scene;
extern double currTime;
extern double prevTime;
extern Camera* Cam;

PlayerObject::PlayerObject() {
	skel = new Skeleton();
	skin = new Skin(skel);
	animation = new Animation();
	animplayer = new Player(skel, animation, std::chrono::steady_clock::now());
	skel->doSkel();
	skin->doSkinning();
	animation->doAnimation();
	particlesystem = nullptr;
}

PlayerObject::PlayerObject(int systemtype) {
	skel = new Skeleton();
	skin = new Skin(skel);
	animation = new Animation();
	animplayer = new Player(skel, animation, std::chrono::steady_clock::now());
	skel->doSkel();
	skin->doSkinning();
	animation->doAnimation();

	if (systemtype == 0) {
		particlesystem = new System();
		particlesystem->InitColoredTrail(glm::vec3(0, 0, 0), glm::vec3(1, 0.1, 0.1));
		psflag = false;
	}
}

void PlayerObject::LoadAnimation() {


	skel->Load((char*)(PROJECT_SOURCE_DIR + std::string("/include/wasp_walk/wasp/wasp.skel")).c_str());
	skin->Load((char*)(PROJECT_SOURCE_DIR + std::string("/include/wasp_walk/wasp/wasp.skin")).c_str());
	animation->Load((char*)(PROJECT_SOURCE_DIR + std::string("/include/wasp_walk/wasp/wasp_dance.anim")).c_str());
}

void PlayerObject::LoadExperimental(std::string filename, int meshindex) {
	std::cout << "entered create" << std::endl;
	Assimp::Importer importer;
	const aiScene* iscene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_PopulateArmatureData);
	if (!iscene || iscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !iscene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	std::cout << "Bones: " << iscene->mMeshes[1]->mNumBones << std::endl;
	std::cout << "Verts: " << iscene->mMeshes[1]->mBones[0]->mNumWeights << std::endl;
	std::cout << "Verts: " << iscene->mMeshes[1]->mBones[1]->mNumWeights << std::endl;
	std::cout << "Verts: " << iscene->mMeshes[1]->mBones[2]->mNumWeights << std::endl;
	std::cout << "children: " << iscene->mMeshes[1]->mBones[0]->mNode->mNumChildren << std::endl;
	std::cout << "children: " << iscene->mMeshes[1]->mBones[1]->mNode->mNumChildren << std::endl;
	std::cout << "children: " << iscene->mMeshes[1]->mBones[2]->mNode->mNumChildren << std::endl;
	std::cout << "total Verts: " << iscene->mMeshes[1]->mNumVertices << std::endl;
	std::cout << "root name " << iscene->mRootNode->mName.C_Str() << std::endl;
	//std::cout << "Skeleton 1 " << iscene->mSkeletons[0]->mNumBones << std::endl;
	//std::cout << "Skeleton 2 " << iscene->mSkeletons[1]->mNumBones << std::endl;

	std::unordered_map<aiNode*, aiBone*> nodeToBone;
	std::cout << "nodes: " << CountNodes(iscene->mRootNode) << std::endl;
	for (int i = 0; i < iscene->mMeshes[meshindex]->mNumBones; i++) {
		//std::cout << "bone: " << iscene->mMeshes[meshindex]->mBones[i]->mName.C_Str() << std::endl;
		//std::cout << "node: " << iscene->mMeshes[meshindex]->mBones[i]->mNode->mName.C_Str() << std::endl;

		nodeToBone[iscene->mMeshes[meshindex]->mBones[i]->mNode] = iscene->mMeshes[meshindex]->mBones[i];
	}

	skin->Preload(iscene->mMeshes[meshindex]->mNumVertices);
	Joint* root = new Joint(skel, nullptr);
	skel->root = root;
	root->Load(iscene->mRootNode, &nodeToBone, skin);
	skin->Load(iscene->mMeshes[meshindex], iscene->mMaterials[iscene->mMeshes[meshindex]->mMaterialIndex]);

}

void PlayerObject::UpdateMat(glm::mat4 newmodel) {
	skel->updateWorldMat(newmodel);
	if (particlesystem) {
		particlesystem->UpdatePos(glm::vec3(newmodel[3]/newmodel[3][3]));
	}
}

void PlayerObject::Update() {
	skel->update();
	if (animation->channels.size() > 0) {
		animplayer->update();
	}
	skin->update();	

	if (currTime != 0 && prevTime != 0 && particlesystem) {
		double deltaTime = currTime - prevTime;
		particlesystem->Update(deltaTime);

		if (!psflag) {

			psflag = true;
		}
	}
}

void PlayerObject::Draw(GLuint shader, bool shadow) {
	//skel->draw(cam->GetViewProjectMtx(), scene->shaders[1]);
	skin->draw(shader, shadow);
	if (particlesystem) {
		particlesystem->Draw(Cam->GetViewProjectMtx(), shader);
	}
	
}

unsigned int CountNodes(const aiNode* node) {
	if (!node) return 0;

	unsigned int count = 1; // Count this node

	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		count += CountNodes(node->mChildren[i]);
	}

	return count;
}