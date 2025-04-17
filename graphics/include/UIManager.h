#pragma once

#include <core.h>
#include "Global.h"
#include "Shader.h"
#include "UIImg.h"

class UIManager {
public:
	void Init();
	void update(const PlayerStats &p);
	void draw();

	void SetGameState(GameState state);
private:
	GameState currState;
	GLuint shaderProgram;
	glm::mat4 projection;
	std::vector<UIImg*> imgElements;

	void UpdateLobby(const PlayerStats &p);
	void DrawLobby();
	void UpdateMatch(const PlayerStats &p);
	void DrawMatch();
};