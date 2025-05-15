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
#include "PlayerObject.h"
#include "Audio.h"

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

//Scene Class which will contain data about map, players, etc
class Scene {
private:
	std::vector<Object*> objects;
	std::vector<Object*> animObjects; //type will be changed later
	std::vector<Cube*> cubes; //for dummy testing
	PlayerObject* player; //type will be changed later
	PlayerObject* test;
	Skybox* skybox; //each player holds a copy of the skybox
	UIManager* uimanager;
	Cube* cube;
	Lights* lightmanager;
	Cube* testCube;
	Audio* audiomanager;
	UIData uiData;
	//std::vector<Light*> lights; lights baked into the world
	//std::vector<Light*> movLights; dynamic lights from abilities/powerups/events etc.

	GLuint depthMapFBO, depthMap;
	glm::mat4 lightSpaceMatrix;
	
public:
	std::vector<GLuint> shaders;
	void createGame();
	bool initShaders();
	void initShadows();
	void loadObjects();
	void update(ClientGame* client);
	void draw(Camera* cam);
	void TriggerAnim(int anim);
};