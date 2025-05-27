#pragma once

#include <core.h>
#include "Global.h"
#include "Shader.h"
#include "UIImg.h"
#include "unordered_map"

class UIManager {
public:
	void Init(ClientGame* client);

	void update(const UIData &p);

	void draw();

	//Texture Manager
	void LoadTexture(const std::string& name, const std::string& path);
	GLuint GetTexture(const std::string& name);
	void UIManager::UnloadAllTextures();

	void NextGamePhase();
	GamePhase GetGamePhase();
	void TriggerAnim(int anim);
	int getPowerup();
private:
	ClientGame* client;
	//GLuint shaderProgram;
	glm::mat4 projection;

	// Add more vectors for the different game states
	std::vector<UIImg*> matchElements;
	std::vector<UIImg*> lobbyElements;
	std::unordered_map<std::string, GLuint> textures;
	
};