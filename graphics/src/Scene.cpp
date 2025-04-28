#include <Scene.h>

PlayerStats dummy;

PlayerObject* players[4];

void Scene::createGame() {
	//setup lights
	lightmanager = new Lights();
	lightmanager->init();

	//loadObjects();
	cube = new Cube();
	//skybox = new Skybox();
	//skybox->initSkybox();

	uimanager = new UIManager;
	uimanager->Init();
	//Necessary for the uimanager, will change once network protocol gets updated
	dummy.maxHP = 250;
	dummy.currHP = dummy.maxHP;
	dummy.ID = 0;
	player = new PlayerObject();
	players[0] = player;

	for (int i = 1; i < 4; i++) {
		players[i] = new PlayerObject();
	}
}

void Scene::loadObjects() {
	Object* obj = new Object();
	std::string importstr = PROJECT_SOURCE_DIR + std::string("/assets/pagoda.obj");
	obj->create((char*)importstr.c_str(), glm::mat4(1), 1);
	objects.push_back(obj);

	//wasp load-in
	player->LoadAnimation();

	for (int i = 1; i < 4; i++) {
		players[i]->LoadAnimation();
	}
}

void Scene::update(ClientGame* client) {
	//this is where game state will be sent to and then recieved from the server. This function can be updated to include parameters that encapsulate
	//player input, so that it can be sent to the server as well
	lightmanager->update();
	//cube->setModel(client->GameState.cubeModel);
	player->Update(client->playerModel);

	int i;
	int j;
	for (i = 0, j = 1; i < client->GameState.num_players; i++) {
		auto entity = client->GameState.players[i];

		if (entity.id == client->playerId) {
			continue;
		}

		players[j++]->Update(entity.model);
	}


	cubes.clear();
	cube->setModel(glm::mat4(1.0f));
	cubes.push_back(cube);
	
	for (i = 0; i < client->GameState.num_entities; i++) {
		auto entity = client->GameState.entities[i];
		if (entity.type == CUBE) {
			Cube* cu = new Cube(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
	}
	

	uimanager->update(dummy);
	//waspplayer->update();
}

bool Scene::initShaders() {
	// Create a shader program with a vertex shader and a fragment shader.
	std::vector<std::string> shadernames = { "texShader", "testShader" };
	
	for (int i = 0; i < shadernames.size(); i++) {
		std::string frag = PROJECT_SOURCE_DIR + std::string("/shaders/") + shadernames[i] + std::string(".frag");
		std::string vert = PROJECT_SOURCE_DIR + std::string("/shaders/") + shadernames[i] + std::string(".vert");
		GLuint shaderProgram = LoadShaders(vert.c_str(), frag.c_str());

		// Check the shader program.
		if (!shaderProgram) {
			std::cerr << "Failed to initialize shader program" << std::endl;
			return false;
		}

		shaders.push_back(shaderProgram);
	}

	return true;
}

void Scene::draw(Camera* cam) {
	//We will use a global shader for everything for right now

	glUseProgram(shaders[1]);
	glUniform1i(glGetUniformLocation(shaders[1], "numLights"), lightmanager->numLights());
	glm::mat4 viewProjMtx = cam->GetViewProjectMtx();
	glUniformMatrix4fv(glGetUniformLocation(shaders[1], "viewProj"), 1, false, (float*)&viewProjMtx);
	glm::vec3 camPos = cam->GetPosition();
	glUniform3fv(glGetUniformLocation(shaders[1], "viewPos"), 1, &camPos[0]);
	lightmanager->bind();
	
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->draw(cam->GetViewProjectMtx());
	}


	for (int i = 0; i < cubes.size(); i++) {
		cubes[i]->draw(cam->GetViewProjectMtx(), shaders[1]);
	}

	//cube->draw(cam->GetViewProjectMtx(), shaders[1]);
	//skel->draw(cam->GetViewProjectMtx(), shaders[1]);
	//skin->draw(cam->GetViewProjectMtx(), shaders[1]);

	for (int i = 0; i < 4; i++) {
		players[i]->Draw(cam);
	}

	
	glUseProgram(0); //skybox and uimanager use their own shader
	
	//ORDER GOES: 3D OBJECTS -> SKYBOX -> UI
	//skybox->draw(cam);
	uimanager->draw();
}