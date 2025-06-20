#pragma once

#include <vector>
#include "network/ClientGame.h"
#include "Skeleton.h"
#include "Skin.h"
#include "Player.h"
#include "Animation.h"
#include <System.h>

//Scene Class which will contain data about map, players, etc
class PlayerObject {
private:
	Skeleton* skel;
	Skin* skin;
	Animation* animation;
	Player* animplayer;
	System* particlesystem;
	System* powerupsystem;
	System* damagesystem;
	bool psflag;
	bool colorflag;
	bool ground;
	bool move;
	
public:
	PlayerObject();
	PlayerObject(int systemtype);
	void enableAnimation();
	void disableAnimation();
	void LoadAnimation();
	void LoadExperimental(std::string filename, int meshindex, int texindex);
	void UpdateMat(glm::mat4 newmodel);
	void Update();
	void UpdateParticles(PlayerStats stats, int id);
	void Draw(GLuint shader, bool shadow);
};