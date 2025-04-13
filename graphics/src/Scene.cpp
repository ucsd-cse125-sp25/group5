#include <Scene.h>

void Scene::createGame() {
	//loadObjects();
	std::cout << "Creating the skybox" << std::endl;
	skybox = new Skybox();
	skybox->initSkybox();
}

void Scene::loadObjects() {

}

void Scene::update() {

}

void Scene::draw(Camera* cam) {
	//Draw skybox last
	skybox->draw(cam);
}