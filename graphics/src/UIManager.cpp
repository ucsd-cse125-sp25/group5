#include "UIManager.h"
#include <iostream>

// Initalizes the base VAOs/VBOs for each state
void UIManager::Init() {
	//Initalize the UI shader
	shaderProgram = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	projection = glm::ortho(0.0f, 1200.0f, 0.0f, 900.0f, -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	//Create new UI elements and populate vector
	HealthBar* hb = new HealthBar();
	hb->Init(1200, 900, { 25.0f, 25.0f }, 0.35, 0.25);
	imgElements.push_back(hb);
	std::cout << "Size of imgElements: " << imgElements.size() << std::endl;
}

void UIManager::update(const PlayerStats& p) {
	switch (currState) {
	case GameState::LOBBY:
		UpdateLobby(p);
		break;
	case GameState::MATCH:
		UpdateMatch(p);
		break;
	}
}

void UIManager::draw() {
	switch (currState) {
		case GameState::LOBBY:
			DrawLobby();
			break;
		case GameState::MATCH:
			DrawMatch();
			break;
	}
}

void UIManager::SetGameState(GameState newState) {
	if (currState == newState) return;
	currState = newState;
}

void UIManager::UpdateLobby(const PlayerStats &p) {
	for (UIImg* img : imgElements) {
		img->Update(p);
	}
}

void UIManager::DrawLobby() {
	glUseProgram(shaderProgram);
	for (UIImg* img : imgElements) {
		img->Draw();
	}
}

void UIManager::UpdateMatch(const PlayerStats &p) {

}
void UIManager::DrawMatch() {

}