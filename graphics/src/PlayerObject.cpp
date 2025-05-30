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

glm::vec3 cor1(0.41, 0.94, 0.95);
glm::vec3 cor2(0.94, 0.64, 0.22);
glm::vec3 cor3(0.93, 0.26, 0.94);
glm::vec3 cor4(0.89, 0.89, 0.89);

glm::vec3 cor5(0.5, 0.5, 0.5);
glm::vec3 cor6(0.32, 0.75, 0.25);
glm::vec3 cor7(0.25, 0.51, 0.76);
glm::vec3 cor8(0.9, 0.24, 0.29);
glm::vec3 cor9(0.675, 0.675, 0.35);

glm::vec3 cores[9] = {cor1, cor2, cor3, cor4, cor5, cor6, cor7, cor8, cor9};

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

		powerupsystem = new System();
		powerupsystem->InitParticleExplosion(glm::vec3(0, 0, 0));

		damagesystem = new System();
		damagesystem->InitDamageEffect(glm::vec3(0, 0, 0));

		psflag = false;
		colorflag = false;

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

	//std::cout << "Bones: " << iscene->mMeshes[1]->mNumBones << std::endl;
	//std::cout << "Verts: " << iscene->mMeshes[1]->mBones[0]->mNumWeights << std::endl;
	//std::cout << "Verts: " << iscene->mMeshes[1]->mBones[1]->mNumWeights << std::endl;
	//std::cout << "Verts: " << iscene->mMeshes[1]->mBones[2]->mNumWeights << std::endl;
	//std::cout << "children: " << iscene->mMeshes[1]->mBones[0]->mNode->mNumChildren << std::endl;
	//std::cout << "children: " << iscene->mMeshes[1]->mBones[1]->mNode->mNumChildren << std::endl;
	//std::cout << "children: " << iscene->mMeshes[1]->mBones[2]->mNode->mNumChildren << std::endl;
	//std::cout << "total Verts: " << iscene->mMeshes[1]->mNumVertices << std::endl;
	//std::cout << "root name " << iscene->mRootNode->mName.C_Str() << std::endl;
	//std::cout << "Skeleton 1 " << iscene->mSkeletons[0]->mNumBones << std::endl;
	//std::cout << "Skeleton 2 " << iscene->mSkeletons[1]->mNumBones << std::endl;

	std::unordered_map<aiNode*, aiBone*> nodeToBone;
	std::cout << "meshes: " << iscene->mNumMeshes << std::endl;
	std::cout << "nodes: " << CountNodes(iscene->mRootNode) << std::endl;
	for (int i = 0; i < iscene->mMeshes[meshindex]->mNumBones; i++) {
		std::cout << "bone: " << iscene->mMeshes[meshindex]->mBones[i]->mName.C_Str() << std::endl;
		std::cout << "node: " << iscene->mMeshes[meshindex]->mBones[i]->mNode->mName.C_Str() << std::endl;

		nodeToBone[iscene->mMeshes[meshindex]->mBones[i]->mNode] = iscene->mMeshes[meshindex]->mBones[i];
	}
	//std::cout << "Printing stuff: " << iscene->mRootNode->mChildren[0]->mChildren[0]->mChildren[0]->mChildren[0]. << std::endl;

	std::cout << "Going to print out the entire bones" << std::endl;
	for (const auto& pair : nodeToBone) {
		std::cout << pair.first->mName.C_Str() << " " << pair.second->mName.C_Str() << std::endl;
	}

	skin->Preload(iscene->mMeshes[meshindex]->mNumVertices);
	Joint* root = new Joint(skel, nullptr);
	skel->root = root;
	//iscene->mRootNode->FindNode("b_Root") for fox
	//iscene->mRootNode->FindNode("rp_manuel_animated_001_dancing_root") for man
	root->Load(iscene->mRootNode->FindNode("rp_manuel_animated_001_dancing_root"), &nodeToBone, skin);
	skin->Load(iscene->mMeshes[meshindex], iscene->mMaterials[iscene->mMeshes[meshindex]->mMaterialIndex]);

	//std::cout << iscene->mRootNode->mChildren[0]->mChildren[0]->mChildren[0] << std::endl;
	//load animations
	std::cout << "NUMBER OF ANIMATIONS: " << iscene->mNumAnimations << std::endl;
	animation->Load(iscene, 0);
}

void PlayerObject::UpdateMat(glm::mat4 newmodel) {
	skel->updateWorldMat(newmodel);
	if (particlesystem) {
		//particlesystem->UpdatePos(glm::vec3(newmodel[3]/newmodel[3][3]));
		particlesystem->UpdatePos(glm::vec3(newmodel[3] / newmodel[3][3]) + glm::vec3(0, -0.1, 0));
		powerupsystem->UpdatePos(glm::vec3(newmodel[3] / newmodel[3][3]) + glm::vec3(0, -0.1, 0));
		damagesystem->UpdatePos(glm::vec3(newmodel[3] / newmodel[3][3]) + glm::vec3(0, -0.1, 0));
	}
}

void PlayerObject::UpdateParticles(PlayerStats stats, int id) {
	if (!colorflag) {
		colorflag = true;
		particlesystem->particlecolor = cores[id];
	}

	if (stats.inAir || stats.moving) {
		particlesystem->creationrate = 15;
	}
	else {
		particlesystem->creationrate = 0;
		particlesystem->ctime = currTime;
	}

	powerupsystem->creationrate = 0;
	for (int i = 0; i < 5; i++) {
		if (stats.movementPowerupFlag[i] > 0) {
			powerupsystem->creationrate = 200;
			powerupsystem->particlecolor = cores[i + 4];
			break;
		}
		else if (stats.attackPowerupFlag[i] > 0) {
			powerupsystem->creationrate = 40;
			powerupsystem->particlecolor = cores[i + 4];
			break;
		}
	}

	if (powerupsystem->creationrate == 0) {
		powerupsystem->ctime = currTime;
	}

	if (stats.damageFlag == true) {
		damagesystem->creationrate = 300;
	}
	else {
		damagesystem->creationrate = 0;
		damagesystem->ctime = currTime;
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
		powerupsystem->Update(deltaTime);
		damagesystem->Update(deltaTime);

		if (!psflag) {

			psflag = true;
		}
	}
}

void PlayerObject::Draw(GLuint shader, bool shadow) {
	//skel->draw(cam->GetViewProjectMtx(), scene->shaders[1]);
	skin->draw(shader, shadow);
	
}

unsigned int CountNodes(const aiNode* node) {
	if (!node) return 0;

	unsigned int count = 1; // Count this node

	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		count += CountNodes(node->mChildren[i]);
	}

	return count;
}