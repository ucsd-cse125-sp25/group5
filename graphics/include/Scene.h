#pragma once

#include <vector>
#include "network/ClientGame.h"
#include "Cube.h"
#include "Object.h"
#include "Skybox.h"
#include "UIManager.h"
#include "Light.h"
#include "Skeleton.h"
#include "Skin.h"
#include "Player.h"
#include "Animation.h"

//Scene Class which will contain data about map, players, etc
class Scene {
private:
	std::vector<Object*> objects;
	std::vector<Object*> animObjects; //type will be changed later
	Object* player; //type will be changed later
	Skybox* skybox; //each player holds a copy of the skybox
	UIManager* uimanager;
	Cube* cube;
	Lights* lightmanager;
	//std::vector<Light*> lights; lights baked into the world
	//std::vector<Light*> movLights; dynamic lights from abilities/powerups/events etc.

	Skeleton* skel;
	Skin* skin;
	Animation* animation;
	Player* waspplayer;
	
public:
	std::vector<GLuint> shaders;
	void createGame();
	bool initShaders();
	void loadObjects();
	void update(ClientGame* client);
	void draw(Camera* cam);
};