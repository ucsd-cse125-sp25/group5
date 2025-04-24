#include "UIManager.h"
#include <iostream>
#include "stb_image.h"

/**
* UIStorage is a map that holds the texture configurations for UI elements
* The data is a tuple containing:
* - path to the texture (std::string)
* - Game state it should appear during (GameState)
* - start position x (float)
* - start position y (float)
* - The width as a percentage of screen width (float)
* - The aspect ratio of the texture (float)
**/
static std::unordered_map<std::string, std::tuple<std::string, GameState, float, float, float, float>> UIStorage = {
		{ "healthbar", { PROJECT_SOURCE_DIR + std::string("/assets/healthbar.png"), GameState::MATCH, 0.0f, 0.0f, 0.5f, 0.15f } },
		{ "avatar", {PROJECT_SOURCE_DIR + std::string("/assets/avatar_back_red.png"), GameState::MATCH, 100.0f, 100.0f, 0.35f, 0.5f} },
		{ "victory", {PROJECT_SOURCE_DIR + std::string("/assets/end_victory.png"), GameState::MATCH, 600.0f, 300.0f, 0.35f, 0.5f} },
		{ "defeat", {PROJECT_SOURCE_DIR + std::string("/assets/end_defeat.png"), GameState::MATCH, 550.0f, 650.0f, 0.35f, 0.5f} }
};

//Loads textures and creates UI elements
void UIManager::Init() {
	float scWidth = 1200, scHeight = 900;

	for (const auto& pair : UIStorage) {
		const std::string& name = pair.first;
		const std::string& path = std::get<0>(pair.second);
		GameState state = std::get<1>(pair.second);
		float startX = std::get<2>(pair.second);
		float startY = std::get<3>(pair.second);
		float percent = std::get<4>(pair.second);
		float aspect = std::get<5>(pair.second);

		LoadTexture(name, path);
		UIImg* img;
		if (name == "healthbar") {
			img = new HealthBar();
		}
		else {
			img = new UIImg();
		}
		img->Init(scWidth, scHeight, { startX, startY }, percent, aspect);
		img->SetTexture(GetTexture(name));


		switch (state) {
		case GameState::LOBBY:
			lobbyElements.push_back(img);
			break;
		case GameState::MATCH:
			matchElements.push_back(img);
			break;
		}
		
	}
}

void UIManager::update(const PlayerStats& p) {
	switch (currState) {
	case GameState::LOBBY:
		for (auto* img : matchElements) {
			img->Update(p);
		}
		break;
	case GameState::MATCH:
		for (auto* img : matchElements) {
			img->Update(p);
		}
		break;
	}
}

void UIManager::draw() {
	switch (currState) {
		case GameState::LOBBY:
			for (auto* img : matchElements) {
				img->Draw();
			}
			break;
		case GameState::MATCH:
			for (auto* img : matchElements) {
				img->Draw();
			}
			break;
	}
}



void UIManager::LoadTexture(const std::string &name, const std::string &path) {
	//Check if texture is already loaded
	if (textures.count(name)) {
		return; //no need to load same texture
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (data) {
		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cerr << "Failed to load texture: " << path << std::endl;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	textures[name] = textureID;
}


GLuint UIManager::GetTexture(const std::string& name) {
	if (!textures.count(name)) {
		std::cerr << "Texture not found: " << name << std::endl;
		return 0;
	}
	return textures[name];
}

void UIManager::UnloadAllTextures() {
	for (auto& pair : textures) {
		glDeleteTextures(1, &pair.second);
	}
	textures.clear();
}

void UIManager::SetGameState(GameState newState) {
	if (currState == newState) return;
	currState = newState;
}
