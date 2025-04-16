#pragma once

#include <vector>
#include "Object.h"
#include "Skybox.h"

//Scene Class which will contain data about map, players, etc
class Scene {
private:
	std::vector<Object*> objects;
	std::vector<Object*> players; //this will be changed to AnimObject* or Player*
	std::vector<GLuint> shaders;
	Skybox* skybox; //each player holds a copy of the skybox
	//std::vector<Lights>; ??? maybe
public:
	void createGame();
	void initShaders();
	void loadObjects();
	void update();
	void draw(Camera* cam);
};