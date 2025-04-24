#pragma once

#include <core.h>
#include "Global.h"
#include "Shader.h"
#include "UIImg.h"
#include "unordered_map"

class UIManager {
public:
	void Init();
	void update(const PlayerStats &p);
	void draw();

	//Texture Manager
	void LoadTexture(const std::string& name, const std::string& path);
	GLuint GetTexture(const std::string& name);
	void UIManager::UnloadAllTextures();

	void SetGameState(GameState state);

	GameState currState = GameState::MATCH;
private:
	//GLuint shaderProgram;
	glm::mat4 projection;

	// Add more vectors for the different game states
	std::vector<UIImg*> matchElements;
	std::vector<UIImg*> lobbyElements;
	std::unordered_map<std::string, GLuint> textures;
};