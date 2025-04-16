#include "UIManager.h"
#include <iostream>

// Initalizes the base VAOs/VBOs for each state
void UIManager::Init() {
	//Load textures on the quads
	shaderProgram = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
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
	//The logic of HP will move to match, right now its in lobby since theres no logic from server of state :)
	maxHP = p.maxHP;
	currHP = p.currHP;
	std::cout << "MAX HP: " << maxHP << std::endl;
	std::cout << "CURRENT HP:" << currHP << std::endl;
}

void UIManager::DrawLobby() {
	//std::cout << "DRAWING LOBBY" << std::endl;
	if (maxHP <= 0) {
		return;
	}
	float windowWidth = 1200, windowHeight = 900;
	float barWidth = 200.0f, barHeight = 50.0f;
	float perc = float(currHP) / float(maxHP);	

	glm::mat4 projection = glm::ortho(0.0f, windowWidth, 0.0f, windowHeight, -1.0f, 1.0f);

	float grayVertices[] = {
		// Position       // Color
		0.0f,   0.0f,             1.0f, 1.0f, 1.0f,
		barWidth, 0.0f,           1.0f, 1.0f, 1.0f,
		barWidth, barHeight,      1.0f, 1.0f, 1.0f,
		0.0f,   barHeight,        1.0f, 1.0f, 1.0f,
	};

	float redVertices[] = {
		0.0f,   0.0f,              0.3f, 0.0f, 0.0f,
		barWidth * perc, 0.0f,     0.3f, 0.0f, 0.0f,
		barWidth * perc, barHeight,   0.3f, 0.0f, 0.0f,
		0.0f,   barHeight,            0.3f, 0.0f, 0.0f,
	};

	GLuint VAO[2], VBO[2], EBO;
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//grey quad
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glBufferData(GL_ARRAY_BUFFER, sizeof(grayVertices), grayVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//red quad
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glBufferData(GL_ARRAY_BUFFER, sizeof(redVertices), redVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(VAO[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Draw red bar
	glBindVertexArray(VAO[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void UIManager::UpdateMatch(const PlayerStats &p) {

}
void UIManager::DrawMatch() {

}