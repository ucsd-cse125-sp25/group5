#include <Scene.h>
#include <Water.h>
#include "Global.h"

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

int moonphase = 0;
bool phasechange = false;

float soundcooldown = 0.4f;
const char* attackKeys[] = { "metalA", "woodA", "waterA", "fireA", "earthA"};
const char* movementKeys[] = { "metalM", "woodM", "waterM", "fireM", "earthM"};
static bool prevAttackFlags[MAX_PLAYERS][5] = { false };
static bool prevMovementFlags[MAX_PLAYERS][5] = { false };
static float lastUsedAttack[MAX_PLAYERS][5] = { 0.0f };
static float lastUsedMovement[MAX_PLAYERS][5] = { 0.0f };



void Scene::createGame(ClientGame* client) {
	this->client = client;

	grapple = new Cube(glm::vec3(-0.1f, -0.1f, -0.1f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.55f, 0.0f));
	grapple->setModel(glm::mat4(1.0f));

	shrink = glm::scale(shrink, glm::vec3(0.05f));

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
	watermat[3] = glm::vec4(-75.0, 0, -75.0, 1);
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
	for (int i = 0; i < mapObjects.size(); i++) {
		auto entry = mapObjects[i];
		std::string file_name = std::get<0>(entry);
		glm::vec3 position = std::get<1>(entry);
		std::string extension = std::get<2>(entry);
		Object* obj = new Object();
		std::string statObj = PROJECT_SOURCE_DIR + std::string("/assets/") + file_name + extension;
		glm::mat4 id = glm::mat4(1.0f);
		id[3] = glm::vec4(position, 1.0f);
		obj->create((char*)statObj.c_str(), id, 1);
		objects.push_back(obj);
	}

	flag = new Object();
	std::string importstr2 = PROJECT_SOURCE_DIR + std::string("/assets/flag.obj");
	flag->create((char*)importstr2.c_str(), glm::mat4(1), 1);
	//objects.push_back(flag);

	metalpower = new Object();
	std::string importstr3 = PROJECT_SOURCE_DIR + std::string("/assets/metal.fbx");
	metalpower->create((char*)importstr3.c_str(), glm::mat4(1), 1);

	metalring = new Object();
	std::string importstr4 = PROJECT_SOURCE_DIR + std::string("/assets/Metal_power.fbx");
	metalring->create((char*)importstr4.c_str(), glm::mat4(1), 1);

	woodpower = new Object();
	std::string importstr5 = PROJECT_SOURCE_DIR + std::string("/assets/wood.obj");
	woodpower->create((char*)importstr5.c_str(), glm::mat4(1), 1);

	woodring = new Object();
	std::string importstr6 = PROJECT_SOURCE_DIR + std::string("/assets/Wood_Power.fbx");
	woodring->create((char*)importstr6.c_str(), glm::mat4(1), 1);

	waterpower = new Object();
	std::string importstr7 = PROJECT_SOURCE_DIR + std::string("/assets/water.fbx");
	waterpower->create((char*)importstr7.c_str(), glm::mat4(1), 1);

	waterring = new Object();
	std::string importstr8 = PROJECT_SOURCE_DIR + std::string("/assets/Water_Power.fbx");
	waterring->create((char*)importstr8.c_str(), glm::mat4(1), 1);

	firepower = new Object();
	std::string importstr9 = PROJECT_SOURCE_DIR + std::string("/assets/fire.obj");
	firepower->create((char*)importstr9.c_str(), glm::mat4(1), 1);

	firering = new Object();
	std::string importstr10 = PROJECT_SOURCE_DIR + std::string("/assets/Fire_power.fbx");
	firering->create((char*)importstr10.c_str(), glm::mat4(1), 1);

	earthpower = new Object();
	std::string importstr11 = PROJECT_SOURCE_DIR + std::string("/assets/earth.obj");
	earthpower->create((char*)importstr11.c_str(), glm::mat4(1), 1);

	earthring = new Object();
	std::string importstr12 = PROJECT_SOURCE_DIR + std::string("/assets/Earth_power.fbx");
	earthring->create((char*)importstr12.c_str(), glm::mat4(1), 1);


	//test->LoadExperimental(PROJECT_SOURCE_DIR + std::string("/assets/man.fbx"), 1);
	
	//test->UpdateMat(mov);
	//wasp load-in
	player->LoadAnimation();
	for (int i = 1; i < 4; i++) {
		players[i]->LoadAnimation();
	}
	lastFrameTime = glfwGetTime();
}


void Scene::update(Camera* cam) {
	//this is where game state will be sent to and then recieved from the server. This function can be updated to include parameters that encapsulate
	//player input, so that it can be sent to the server as well
	lightmanager->update();
	lightSpaceMatrix = lightmanager->getDirLightMat();

	player->UpdateMat(client->playerModel);
	player->UpdateParticles(client->GameState.player_stats[client->playerId], client->playerId);
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
	projectiles.clear();

	//set the height of the water
	glm::mat4 watermat(1);
	watermat[3] = glm::vec4(-75.0, waterLevel + 2.0f, -75.0, 1);
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
			Cube* cu = new Cube(island_min, island_max, glm::vec3(0.4f, 0.8f, 0.5f));
			cu->setModel(entity.model);
			cubes.push_back(cu);
		}
		else if (entity.type == FLAG) {
			if (flag != NULL) {
				entity.model = glm::scale(entity.model, glm::vec3(0.8f));
				flag->update(entity.model);
			}
		}
		else if (entity.type == WOOD_PROJ) {
			Projectile p;
			p.power = WOOD;
			p.model = entity.model;
			projectiles.push_back(p);
		}
		else if (entity.type == METAL_PROJ) {
			Projectile p;
			p.power = METAL;
			p.model = entity.model;
			projectiles.push_back(p);

		}
		else if (entity.type == WATER_PROJ) {
			Projectile p;
			p.power = WATER;
			p.model = entity.model;
			projectiles.push_back(p);
		}
		else if (entity.type == FIRE_PROJ) {
			Projectile p;
			p.power = FIRE;
			p.model = entity.model;
			projectiles.push_back(p);
		}
		else if (entity.type == EARTH_PROJ) {
			Projectile p;
			p.power = EARTH;
			p.model = entity.model;
			projectiles.push_back(p);
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
	dummy.hasFlag = client->GameState.player_stats[client->playerId].hasFlag;
	dummy.dealtDamage = client->GameState.player_stats[client->playerId].dealtDamageFlag;

	if (client->GameState.phase == POST_GAME && dummy.hasFlag) {
		audiomanager->selfState = 2;
	}
	if (client->GameState.phase == POST_GAME && !dummy.hasFlag) {
		audiomanager->selfState = 1;
	}
	audiomanager->phase = client->GameState.phase;
	audiomanager->Update(cam, dummy);
	uimanager->update(dummy);

	//This is where we will play the sounds
	for (int i = 0; i < client->GameState.num_players; i++) {
		PlayerStats& c = client->GameState.player_stats[i];
		glm::vec3 pos = client->GameState.players[i].model[3];
		float vol = 0.75f;
		if (client->playerId != client->GameState.players[i].id) {
			vol = 0.45f;
		}
		for (int j = 0; j < 5; j++) {
			float now = glfwGetTime();
			if (c.attackPowerupFlag[j] == 0 || c.attackPowerupFlag[j] > 2) {
				prevAttackFlags[i][j] = false;
			}
			if (c.movementPowerupFlag[j] == 0 || c.movementPowerupFlag[j] > 2) {
				prevMovementFlags[i][j] = false;
			}
			if ((c.attackPowerupFlag[j] == 1 || c.attackPowerupFlag[j] == 2) && !prevAttackFlags[i][j] && attackKeys[j]) {
				if (now - lastUsedAttack[i][j] > soundcooldown) {
					audiomanager->PlayAudio(attackKeys[j], pos, vol);
					lastUsedAttack[i][j] = now;
				}
				prevAttackFlags[i][j] = true;
			}
			if ((c.movementPowerupFlag[j] == 1 || c.movementPowerupFlag[j] == 2) && !prevMovementFlags[i][j] && movementKeys[j]) {
				if (now - lastUsedMovement[i][j] > soundcooldown) {
					audiomanager->PlayAudio(movementKeys[j], pos, vol);
					lastUsedMovement[i][j] = now;
				}
				prevMovementFlags[i][j] = true;
			}
		}
	}
	phasechange = false;
	if (moonphase != client->GameState.moonPhase) {
		moonphase = client->GameState.moonPhase;
		phasechange = true;
		skybox->updatePhase(moonphase);
	}
}

bool Scene::initShaders() {
	// Create a shader program with a vertex shader and a fragment shader.
	std::vector<std::string> shadernames = { "texShader", "testShader", "shadow", "particleShader", "waterShader", "spectral"};
	
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

	DirectionalLight* dirLight = lightmanager->getDirLight();
	//update dirLight for moon phase
	if (phasechange) {
		if (moonphase == 1) {
			dirLight->color = glm::vec3(0.7, 0.75, 0.68) / 2.5f;
			dirLight->direction = glm::normalize(glm::vec3(-1.0, 0.48, 0.0));
			dirLight->specular = glm::vec3(0.7, 0.75, 0.68) / 1.67f;
		}
		else if (moonphase == 2) {
			dirLight->color = glm::vec3(0.7, 0.75, 0.68) / 2.0f;
			dirLight->direction = glm::normalize(glm::vec3(-1.0, 0.52, 0.07));
			dirLight->specular = glm::vec3(0.7, 0.75, 0.68) / 1.33f;
		}
	}

	glUniform3fv(glGetUniformLocation(mainShader, "dirLightDir"), 1, &dirLight->direction[0]);
	glUniform3fv(glGetUniformLocation(mainShader, "dirLightColor"), 1, &dirLight->color[0]);
	glUniform3fv(glGetUniformLocation(mainShader, "dirLightSpec"), 1, &dirLight->specular[0]);
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
	glUniform4fv(glGetUniformLocation(mainShader, "waterModel"), 1, (float*)&water->model);

	lightmanager->bind();
	
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->draw(mainShader, false);
	}

	for (int i = 0; i < cubes.size(); i++) {
		cubes[i]->draw(mainShader, false);
	}

	for (int i = 0; i < projectiles.size(); i++) {
		Projectile p = projectiles.at(i);
		if (p.power == METAL) {
			metalpower->update(glm::scale(p.model, glm::vec3(0.05f)));
			metalpower->draw(mainShader, false);
		}
		else if (p.power == WOOD) {
			woodpower->update(p.model);
			woodpower->draw(mainShader, false);
		}
		else if (p.power == WATER) {
			waterpower->update(glm::rotate(p.model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
			waterpower->draw(mainShader, false);
		}
		else if (p.power == FIRE) {
			firepower->update(p.model);
			firepower->draw(mainShader, false);
		}
		else if (p.power == EARTH) {
			earthpower->update(p.model);
			earthpower->draw(mainShader, false);
		}
	}
	//test->Draw(mainShader, false);

	float now = glfwGetTime();
	float deltaTime = now - lastFrameTime;
	lastFrameTime = now;
	ringRot += 180.0f * deltaTime;
	ringRot = fmod(ringRot, 360.0f);

	for (int i = 1; i < client->GameState.num_players; i++) {
		players[i]->Draw(mainShader, false);
	}

	for (int i = 0; i < client->GameState.num_players; i++) {
		if (client->GameState.players[i].id == client->playerId) {
			continue;
		}
		glm::vec3 pos = client->GameState.players[i].model[3];
		std::cout << i << std::endl;
		std::cout << glm::to_string(pos) << std::endl;
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::scale(matrix, glm::vec3(0.05f));
		matrix[3] = glm::vec4(pos, 1.0f);
		matrix = glm::rotate(matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		matrix = glm::rotate(matrix, glm::radians(ringRot), glm::vec3(0.0f, 0.0f, 1.0f));
		PowerType active = client->GameState.player_stats[i].activePower;

		if (active == METAL) {
			metalring->update(matrix);
			metalring->draw(mainShader, false);
		}
		else if (active == WOOD) {
			glm::mat4 matrix = glm::mat4(1.0f);
			matrix = glm::scale(matrix, glm::vec3(0.2f));
			matrix[3] = glm::vec4(pos, 1.0f);
			matrix = glm::rotate(matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			matrix = glm::rotate(matrix, glm::radians(ringRot), glm::vec3(0.0f, 0.0f, 1.0f));
			woodring->update(matrix);
			woodring->draw(mainShader, false);
		}
		else if (active == WATER) {
			waterring->update(matrix);
			waterring->draw(mainShader, false);
		}
		else if (active == FIRE) {
			glm::mat4 matrix = glm::mat4(1.0f);
			matrix = glm::scale(matrix, glm::vec3(0.15f));
			matrix[3] = glm::vec4(pos, 1.0f);
			matrix = glm::rotate(matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			matrix = glm::rotate(matrix, glm::radians(ringRot), glm::vec3(0.0f, 0.0f, 1.0f));
			firering->update(matrix);
			firering->draw(mainShader, false);
		}
		else if (active == EARTH) {
			earthring->update(matrix);
			earthring->draw(mainShader, false);
		}
	}


	grapple->draw(mainShader, false);
	for (int i = 0; i < client->GameState.num_players; i++) {
		if (client->GameState.player_stats[i].grappleTarget != glm::vec3(0.0f)) {
			glm::vec3 pos = client->GameState.players[i].model[3];
			glm::vec3 end = client->GameState.player_stats[i].grappleTarget;
			glLineWidth(10.0f);
			glBegin(GL_LINES);
			glVertex3f(pos.x, pos.y - 0.15f, pos.z);
			glVertex3f(end.x, end.y, end.z);
			glEnd();
		}
	}

	glDepthFunc(GL_GREATER);
	glDepthMask(GL_FALSE);
	GLuint spectral = shaders[5];
	glUseProgram(spectral);
	glUniformMatrix4fv(glGetUniformLocation(spectral, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
	glUniform1f(glGetUniformLocation(spectral, "uScale"), 1.02f);
	flag->draw(spectral, false);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glUseProgram(mainShader);
	glUniformMatrix4fv(glGetUniformLocation(mainShader, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
	glUniform3fv(glGetUniformLocation(mainShader, "viewPos"), 1, &camPos[0]);
	glUniform3fv(glGetUniformLocation(mainShader, "dirLightDir"), 1, &dirLight->direction[0]);
	glUniform3fv(glGetUniformLocation(mainShader, "dirLightColor"), 1, &dirLight->color[0]);
	glUniform3fv(glGetUniformLocation(mainShader, "dirLightSpec"), 1, &dirLight->specular[0]);
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
	glUniform4fv(glGetUniformLocation(mainShader, "waterModel"), 1, (float*)&water->model);
	lightmanager->bind();
	flag->draw(mainShader, false);

	glDisable(GL_CULL_FACE);

	//water shading and drawing
	GLuint waterShader = shaders[4];
	glUseProgram(waterShader);

	glUniformMatrix4fv(glGetUniformLocation(waterShader, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
	glUniform3fv(glGetUniformLocation(waterShader, "viewPos"), 1, &camPos[0]);

	glUniform3fv(glGetUniformLocation(waterShader, "dirLightDir"), 1, &dirLight->direction[0]);
	glUniform3fv(glGetUniformLocation(waterShader, "dirLightColor"), 1, &dirLight->color[0]);
	glUniform3fv(glGetUniformLocation(waterShader, "dirLightSpec"), 1, &dirLight->specular[0]);
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
	glUniform3fv(glGetUniformLocation(particleShader, "viewPos"), 1, &camPos[0]);

	for (int i = 0; i < particlesystems.size(); i++) {
		particlesystems[i]->Draw(particleShader);
	}
  
	glUseProgram(0); //skybox and uimanager use their own shader
	
	//ORDER GOES: 3D OBJECTS -> SKYBOX -> UI
	skybox->draw(cam, (float*)&water->model);
	uimanager->draw();
}

void Scene::TriggerAnim(int anim) {
	uimanager->TriggerAnim(anim);
}

int Scene::getPowerup() {
	return uimanager->getPowerup();
}