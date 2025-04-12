#include <Scene.h>

void Scene::createGame() {
	this->loadObjects();

	skybox = new Skybox();
	skybox->initSkybox();
}

void Scene::loadObjects() {

}

void Scene::update() {

}

void Scene::draw() {
	//Draw skybox last

}