#pragma once

#include <vector>
#include "network/ClientGame.h"
#include "Skeleton.h"
#include "Skin.h"
#include "Player.h"
#include "Animation.h"

//Scene Class which will contain data about map, players, etc
class PlayerObject {
private:
	Skeleton* skel;
	Skin* skin;
	Animation* animation;
	Player* animplayer;
	
public:
	PlayerObject();
	void LoadAnimation();
	void LoadExperimental(std::string filename, int meshindex);
	void UpdateMat(glm::mat4 newmodel);
	void Update();
	void Draw(GLuint shader, bool shadow);
};