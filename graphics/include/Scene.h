#pragma once

#include <vector>
#include "Object.h"
#include "Skybox.h"

//Scene Class which will contain data about map, players, etc
class Scene {
private:
	std::vector<Object*> objects;
	std::vector<Object*> animObjects; //type will be changed later
	Object* player; //type will be changed later
	Skybox* skybox; //each player holds a copy of the skybox
	//std::vector<Lights>; //TBD once we add support for lights
public:
	std::vector<GLuint> shaders;
	void createGame();
	bool initShaders();
	void loadObjects();
	void update();
	void draw(Camera* cam);
};