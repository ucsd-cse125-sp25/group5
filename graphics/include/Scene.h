#pragma once

#include <vector>
#include "Object.h"
#include "Skybox.h"
#include "UIManager.h"

//Scene Class which will contain data about map, players, etc
class Scene {
private:
	std::vector<Object*> objects;
	std::vector<Object*> animObjects; //type will be changed later
	Object* player; //type will be changed later
	Skybox* skybox; //each player holds a copy of the skybox
	UIManager* uimanager;
	//std::vector<Light*> lights; lights baked into the world
	//std::vector<Light*> movLights; dynamic lights from abilities/powerups/events etc.
public:
	std::vector<GLuint> shaders;
	void createGame();
	bool initShaders();
	void loadObjects();
	void update();
	void draw(Camera* cam);
};