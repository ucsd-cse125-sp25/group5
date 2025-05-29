#include <Scene.h>
#include <Water.h>

UIData dummy;

PlayerObject* players[4];

std::vector<System*> particlesystems;

extern double currTime;
extern double startTime;
int WINDOWHEIGHT = 1440;
int WINDOWWIDTH = 2560;
//2560
//1440

float waterLevel = -2.0f;
float fogConstant = 0.01f;
float fogConstantW = 0.075f;
glm::vec3 fogColor(0.35, 0.4, 0.55);
glm::vec3 fogColorW(0.1, 0.2, 0.6);

float soundcooldown = 0.5f;
const char* attackKeys[] = { nullptr, nullptr, "waterA", "fireA", nullptr };
const char* movementKeys[] = { nullptr, nullptr, "waterM", "fireM", nullptr };
static bool prevAttackFlags[MAX_PLAYERS][5] = { false };
static bool prevMovementFlags[MAX_PLAYERS][5] = { false };
static float lastUsedAttack[MAX_PLAYERS][5] = { 0.0f };
static float lastUsedMovement[MAX_PLAYERS][5] = { 0.0f };



void Scene::createGame(ClientGame *client) {
	this->client = client;

	//setup lights
	lightmanager = new Lights();
	lightmanager->init();
	initShadows();

	cube = new Cube();
	skybox = new Skybox();
	skybox->initSkybox();

	uimanager = new UIManager;
	uimanager->Init(client);

	audiomanager = new Audio;
	audiomanager->Init();
	//audiomanager->PlayAudio("matchsong");
	//audiomanager->PlayAudio("firesound");
	test = new PlayerObject();

	//Cinema
	player = new PlayerObject(0);
	players[0] = player;

	for (int i = 1; i < 4; i++) {
		players[i] = new PlayerObject();
	}

	water = new Water();
	water->create(301, 301, 0.5f, waterLevel);
	glm::mat4 watermat(1);
	watermat[3] = glm::vec4(-25.0, 0, -25.0, 1);
	water->update(watermat);

	for (int i = 0; i < MAX_PLAYERS; i++) {
		for (int j = 0; j < 5; j++) {
			lastUsedAttack[i][j] = glfwGetTime();
			lastUsedMovement[i][j] = glfwGetTime();
		}
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}


void Scene::loadObjects() {
	Object* obj = new Object();
	std::string importstr = PROJECT_SOURCE_DIR + std::string("/assets/island.obj");
	obj->create((char*)importstr.c_str(), glm::mat4(1), 1);
	objects.push_back(obj);

	flag = new Object();
	std::string importstr2 = PROJECT_SOURCE_DIR + std::string("/assets/flag.obj");
	flag->create((char*)importstr2.c_str(), glm::mat4(1), 1);
	objects.push_back(flag);
	//test->LoadExperimental(PROJECT_SOURCE_DIR + std::string("/assets/man.fbx"), 1);
	
	//test->UpdateMat(mov);
	//wasp load-in
	player->LoadAnimation();
	for (int i = 1; i < 4; i++) {
		players[i]->LoadAnimation();
	}
}


void Scene::update(Camera* cam) {
	//this is where game state will be sent to and then recieved from the server. This function can be updated to include parameters that encapsulate
	//player input, so that it can be sent to the server as well
	lightmanager->update();
	lightSpaceMatrix = lightmanager->getDirLightMat();

	player->UpdateMat(client->playerModel);
	player->Update();

	//test->Update();
	for (int i = 0; i < KILLFEED_LENGTH; i++) {
		dummy.killfeed[i] = client->GameState.killfeed[i];
	}

	int i;
	int j;
	for (i = 0, j = 1; i < client->GameState.num_players; i++) {
		auto entity = client->GameState.players[i];

		if (entity.id == client->playerId) {
			continue;
		}

		players[j]->UpdateMat(entity.model);
		players[j++]->Update();
	}
	
	for (int i = 0; i < cubes.size(); i++) {
		delete(cubes[i]);
	}
	cubes.clear();

	//set the height of the water
	glm::mat4 watermat(1);
	watermat[3] = glm::vec4(-25.0, client->GameState.waterLevel, -25.0, 1);
	water->update(watermat);
	waterLevel = client->GameState.waterLevel;
	
	for (i = 0; i < client->GameState.num_entities; i++) {
		auto entity = client->GameState.entities[i];
		if (entity.type == CUBE) {
			Cube* cu = new Cube(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == ISLAND) {
			glm::vec3 island_min = glm::vec3(0, 0, 0);
			island_min -= island_extents;
			glm::vec3 island_max = glm::vec3(0, 0, 0);
			island_max += island_extents;

			//printf("Island Min: %f %f %f\n", island_min.x, island_min.y, island_min.z);	
			//printf("Island Max: %f %f %f\n", island_max.x ,island_max.y, island_max.z);
			
			Cube* cu = new Cube(island_min, island_max, glm::vec3(0.4f, 0.8f, 0.5f));
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == FLAG) {
			if (flag != NULL) {
				flag->update(entity.model);
			}
		}
		else if (entity.type == WOOD_PROJ) {
			Cube* cu = new Cube(woodProjExtents, -woodProjExtents, glm::vec3(0.3f, 0.8f, 0.2f));
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == METAL_PROJ) {
			Cube* cu = new Cube(woodProjExtents, -woodProjExtents, glm::vec3(0.5f, 0.5f, 0.5f));
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == WATER_PROJ) {
			Cube* cu = new Cube(woodProjExtents, -woodProjExtents, glm::vec3(0.2f, 0.4f, 1.0f)); // Blue-ish
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == FIRE_PROJ) {
			Cube* cu = new Cube(fireProjExtents, -fireProjExtents, glm::vec3(1.0f, 0.3f, 0.1f)); // Fiery orange
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == EARTH_PROJ) {
			Cube* cu = new Cube(woodProjExtents, -woodProjExtents, glm::vec3(0.4f, 0.3f, 0.1f)); // Brown/soil tone
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == COLLIDER) {
			//generate a random color

			Cube* cu = new Cube(-entity.ext, entity.ext, glm::vec3(0.0f, 1.0f, 0.0f));
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == HP_PICKUP) {
			Cube* cu = new Cube(woodProjExtents, -woodProjExtents, glm::vec3(1.0f, 0.0f, 0.0f)); // Red for HP pickup
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == MANA_PICKUP) {
			Cube* cu = new Cube(woodProjExtents, -woodProjExtents, glm::vec3(0.0f, 1.0f, 1.0f)); // Cyan for Mana pickup
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
	}


	dummy.currMetal = client->GameState.player_stats[client->playerId].mana[0];
	dummy.currWood = client->GameState.player_stats[client->playerId].mana[1];
	dummy.currWater = client->GameState.player_stats[client->playerId].mana[2];
	dummy.currFire = client->GameState.player_stats[client->playerId].mana[3];
	dummy.currEarth = client->GameState.player_stats[client->playerId].mana[4];
	dummy.currHP = client->GameState.player_stats[client->playerId].hp;
	dummy.seconds = client->GameState.time;
	audiomanager->Update(cam, dummy);
	uimanager->update(dummy);

	//This is where we will play the sounds
	for (int i = 0; i < client->GameState.num_players; i++) {
		PlayerStats& c = client->GameState.player_stats[i];
		glm::vec3 pos = client->GameState.players[i].model[3];
		for (int j = 0; j < 5; j++) {
			float now = glfwGetTime();
			if (c.attackPowerupFlag[j] == 0 || c.attackPowerupFlag[j] > 2) {
				prevAttackFlags[i][j] = false;
			}
			if (c.movementPowerupFlag[j] == 0 || c.movementPowerupFlag[j] > 2) {
				prevMovementFlags[i][j] = false;
			}
			if ((c.attackPowerupFlag[j] == 1 || c.attackPowerupFlag[j] == 2) && !prevAttackFlags[i][j] && attackKeys[j]) {
				std::cout << "Going to play audio for player:  " << i << " power: " << j << std::endl;
				if (now - lastUsedAttack[i][j] > soundcooldown) {
					std::cout << "This sound is off cooldown so we can play it!" << std::endl;
					audiomanager->PlayAudio(attackKeys[j], pos);
					lastUsedAttack[i][j] = now;
				}
				prevAttackFlags[i][j] = true;
			}
			if ((c.movementPowerupFlag[j] == 1 || c.movementPowerupFlag[j] == 2) && !prevMovementFlags[i][j] && movementKeys[j]) {
				std::cout << "Going to play audio for player:  " << i << " movement: " << j << std::endl;
				if (now - lastUsedMovement[i][j] > soundcooldown) {
					std::cout << "This sound is off cooldown so we can play it!" << std::endl;
					audiomanager->PlayAudio(movementKeys[j], pos);
					lastUsedMovement[i][j] = now;
				}
				prevMovementFlags[i][j] = true;
			}
		}
	}
}

bool Scene::initShaders() {
	// Create a shader program with a vertex shader and a fragment shader.
	std::vector<std::string> shadernames = { "texShader", "testShader", "shadow", "particleShader", "waterShader"};
	
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

void Scene::initShadows() {
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::draw(Camera* cam) {
	//SHADOW PASS
	bool doShadow = false;
	if (doShadow) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		GLuint shadowShader = shaders[2];

		glUseProgram(shadowShader);
		glUniformMatrix4fv(glGetUniformLocation(shadowShader, "lightSpaceMatrix"), 1, GL_FALSE, (float*)&lightSpaceMatrix);

		for (int i = 0; i < objects.size(); i++) {
			objects[i]->draw(shadowShader, doShadow);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//RENDER PASS
	glViewport(0, 0, WINDOWWIDTH, WINDOWHEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//We will use a global shader for everything for right now
	GLuint mainShader = shaders[1];
	glUseProgram(mainShader);

	glm::mat4 viewProjMtx = cam->GetViewProjectMtx();
	glUniformMatrix4fv(glGetUniformLocation(mainShader, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
	glm::vec3 camPos = cam->GetPosition();
	glUniform3fv(glGetUniformLocation(mainShader, "viewPos"), 1, &camPos[0]);

	DirectionalLight dirLight = lightmanager->getDirLight();
	glUniform3fv(glGetUniformLocation(mainShader, "dirLightDir"), 1, &dirLight.direction[0]);
	glUniform3fv(glGetUniformLocation(mainShader, "dirLightColor"), 1, &dirLight.color[0]);
	glUniform3fv(glGetUniformLocation(mainShader, "dirLightSpec"), 1, &dirLight.specular[0]);
	glUniform1i(glGetUniformLocation(mainShader, "numLights"), lightmanager->numLights());
	glUniformMatrix4fv(glGetUniformLocation(mainShader, "lightSpaceMatrix"), 1, GL_FALSE, (float*)&lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(glGetUniformLocation(mainShader, "shadowMap"), 1);
	glUniform1i(glGetUniformLocation(mainShader, "useShadow"), doShadow ? true : false);

	glUniform1f(glGetUniformLocation(mainShader, "time"), (currTime - startTime) / 1000.0f);
	glUniform1f(glGetUniformLocation(mainShader, "waterLevel"), waterLevel);
	glUniform1f(glGetUniformLocation(mainShader, "fogConstant"), fogConstant);
	glUniform1f(glGetUniformLocation(mainShader, "fogConstantW"), fogConstantW);
	glUniform3fv(glGetUniformLocation(mainShader, "fogColor"), 1, &fogColor[0]);
	glUniform3fv(glGetUniformLocation(mainShader, "fogColorW"), 1, &fogColorW[0]);

	lightmanager->bind();
	
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->draw(mainShader, false);
	}

	for (int i = 0; i < cubes.size(); i++) {
		cubes[i]->draw(mainShader, false);
	}

	//test->Draw(mainShader, false);

	for (int i = 1; i < 4; i++) {

		players[i]->Draw(mainShader, false);
	}

	glDisable(GL_CULL_FACE);

	//water shading and drawing
	GLuint waterShader = shaders[4];
	glUseProgram(waterShader);

	glUniformMatrix4fv(glGetUniformLocation(waterShader, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
	glUniform3fv(glGetUniformLocation(waterShader, "viewPos"), 1, &camPos[0]);

	glUniform3fv(glGetUniformLocation(waterShader, "dirLightDir"), 1, &dirLight.direction[0]);
	glUniform3fv(glGetUniformLocation(waterShader, "dirLightColor"), 1, &dirLight.color[0]);
	glUniform3fv(glGetUniformLocation(waterShader, "dirLightSpec"), 1, &dirLight.specular[0]);
	glUniform1i(glGetUniformLocation(waterShader, "numLights"), lightmanager->numLights());
	glUniformMatrix4fv(glGetUniformLocation(waterShader, "lightSpaceMatrix"), 1, GL_FALSE, (float*)&lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform1i(glGetUniformLocation(waterShader, "shadowMap"), 1);
	glUniform1i(glGetUniformLocation(waterShader, "useShadow"), doShadow ? true : false);

	glUniform1f(glGetUniformLocation(waterShader, "time"), (currTime - startTime)/1000.0f);
	glUniform1f(glGetUniformLocation(waterShader, "waterLevel"), waterLevel);
	glUniform1f(glGetUniformLocation(waterShader, "fogConstant"), fogConstant);
	glUniform1f(glGetUniformLocation(waterShader, "fogConstantW"), fogConstantW);
	glUniform3fv(glGetUniformLocation(waterShader, "fogColor"), 1, &fogColor[0]);
	glUniform3fv(glGetUniformLocation(waterShader, "fogColorW"), 1, &fogColorW[0]);

	lightmanager->bind();

	water->draw(waterShader, false);

	//All particle effects
	GLuint particleShader = shaders[3];
	glUseProgram(particleShader);
	glUniformMatrix4fv(glGetUniformLocation(particleShader, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);

	for (int i = 0; i < particlesystems.size(); i++) {
		particlesystems[i]->Draw(particleShader);
	}
  
	glUseProgram(0); //skybox and uimanager use their own shader
	
	//ORDER GOES: 3D OBJECTS -> SKYBOX -> UI
	skybox->draw(cam);
	uimanager->draw();
}

void Scene::TriggerAnim(int anim) {
	uimanager->TriggerAnim(anim);
}

int Scene::getPowerup() {
	return uimanager->getPowerup();
}