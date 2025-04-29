#include <Scene.h>

PlayerStats dummy;

void Scene::createGame() {
	//setup lights
	lightmanager = new Lights();
	lightmanager->init();
	initShadows();

	//loadObjects();
	cube = new Cube();
	skybox = new Skybox();
	skybox->initSkybox();

	uimanager = new UIManager;
	uimanager->Init();
	//Necessary for the uimanager, will change once network protocol gets updated
	dummy.maxHP = 250;
	dummy.currHP = dummy.maxHP;
	dummy.ID = 0;
	skel = new Skeleton();
	skin = new Skin(skel);
	animation = new Animation();
	waspplayer = new Player(skel, animation, std::chrono::steady_clock::now());
	skel->doSkel();
	skin->doSkinning();
	animation->doAnimation();

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
}

void Scene::loadObjects() {
	Object* obj = new Object();
	std::string importstr = PROJECT_SOURCE_DIR + std::string("/assets/pagoda.obj");
	obj->create((char*)importstr.c_str(), glm::mat4(1), 1);
	objects.push_back(obj);

	//wasp load-in
	skel->Load((char*)(PROJECT_SOURCE_DIR + std::string("/include/wasp_walk/wasp/wasp.skel")).c_str());
	skin->Load((char*)(PROJECT_SOURCE_DIR + std::string("/include/wasp_walk/wasp/wasp.skin")).c_str());
	animation->Load((char*)(PROJECT_SOURCE_DIR + std::string("/include/wasp_walk/wasp/wasp.anim")).c_str());
}

void Scene::update(ClientGame* client) {
	//this is where game state will be sent to and then recieved from the server. This function can be updated to include parameters that encapsulate
	//player input, so that it can be sent to the server as well
	lightmanager->update();
	lightSpaceMatrix = lightmanager->getDirLightMat();
	//cube->setModel(client->GameState.cubeModel);
	skel->updateWorldMat(client->GameState.cubeModel);
	uimanager->update(dummy);
	skel->update();
	//waspplayer->update();
	skin->update();
}

bool Scene::initShaders() {
	// Create a shader program with a vertex shader and a fragment shader.
	std::vector<std::string> shadernames = { "texShader", "testShader", "shadow" };
	
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
	bool doShadow = true;
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
		skin->draw(shadowShader, doShadow);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//RENDER PASS
	glViewport(0, 0, 1200, 900);
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

	lightmanager->bind();
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->draw(mainShader, false);
	}

	skin->draw(mainShader, false);

	glUseProgram(0); //skybox and uimanager use their own shader
	
	//ORDER GOES: 3D OBJECTS -> SKYBOX -> UI
	skybox->draw(cam);
	uimanager->draw();
}