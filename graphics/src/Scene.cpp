#include <Scene.h>

void Scene::createGame() {
	//loadObjects();
	std::cout << "Creating the skybox" << std::endl;
	skybox = new Skybox();
	skybox->initSkybox();
}

void Scene::loadObjects() {
	Object* obj = new Object();
	std::string importstr = PROJECT_SOURCE_DIR + std::string("/assets/pagoda.obj");
	obj->create((char*)importstr.c_str(), glm::mat4(1), 0);
	objects.push_back(obj);
}

void Scene::update() {
	//this is where game state will be sent to and then recieved from the server. This function can be updated to include parameters that encapsulate
	//player input, so that it can be sent to the server as well
}

bool Scene::initShaders() {
	// Create a shader program with a vertex shader and a fragment shader.
	std::vector<std::string> shadernames = { "texShader" };
	
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
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->draw(cam->GetViewProjectMtx());
	}

	//Draw skybox last
	skybox->draw(cam);
}