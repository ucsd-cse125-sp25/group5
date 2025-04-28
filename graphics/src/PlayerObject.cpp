#include "Object.h"
#include <iostream>
#include <Scene.h>
#include "PlayerObject.h"

extern Scene* scene;

PlayerObject::PlayerObject() {
	skel = new Skeleton();
	skin = new Skin(skel);
	animation = new Animation();
	animplayer = new Player(skel, animation, std::chrono::steady_clock::now());
	skel->doSkel();
	skin->doSkinning();
	animation->doAnimation();
}

void PlayerObject::LoadAnimation() {
	skel->Load((char*)(PROJECT_SOURCE_DIR + std::string("/include/wasp_walk/wasp/wasp.skel")).c_str());
	skin->Load((char*)(PROJECT_SOURCE_DIR + std::string("/include/wasp_walk/wasp/wasp.skin")).c_str());
	animation->Load((char*)(PROJECT_SOURCE_DIR + std::string("/include/wasp_walk/wasp/wasp.anim")).c_str());
}

void PlayerObject::Update(glm::mat4 newmodel) {
	skel->updateWorldMat(newmodel);
	skel->update();
	skin->update();	
}

void PlayerObject::Draw(Camera* cam) {
	//skel->draw(cam->GetViewProjectMtx(), scene->shaders[1]);
	skin->draw(cam->GetViewProjectMtx(), scene->shaders[1]);
}