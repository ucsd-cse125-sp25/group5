#pragma once

#include <core.h>
#include "Global.h"
#include "Shader.h"

enum class GameState {
	LOBBY,
	MATCH
};

class UIManager {
public:
	void Init();
	void update(const PlayerStats &p);
	void draw();

	void SetGameState(GameState state);
private:
	GameState currState;

	GLuint shaderProgram;

	//Have one for each game state
	GLuint VAO, VBO;

	unsigned int maxHP, currHP;

	void UpdateLobby(const PlayerStats &p);
	void DrawLobby();
	void UpdateMatch(const PlayerStats &p);
	void DrawMatch();
};