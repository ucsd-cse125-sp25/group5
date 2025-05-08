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
	{ "magicback", { PROJECT_SOURCE_DIR + std::string("/assets/UIUIUI.png"), GameState::MATCH, 835.0, 0.0, 0.3, 1.0} },
	{ "reticle", {PROJECT_SOURCE_DIR + std::string("/assets/reticle.png"), GameState::MATCH, 600.0, 450.0, 0.05, 1.0}},
};

/**
* MagicUI is a map that holds the textures for each element
* The data is a tuple containing:
* - path to the juice texture (std::string)
* - path to the border texture (std::string)
**/
static std::unordered_map<std::string, std::tuple<std::string, std::string>> MagicUI = {
	{"water", {PROJECT_SOURCE_DIR + std::string("/assets/water_bar.png"), PROJECT_SOURCE_DIR + std::string("/assets/water_outline.png")}},
	{"fire", {PROJECT_SOURCE_DIR + std::string("/assets/fire_bar.png"), PROJECT_SOURCE_DIR + std::string("/assets/fire_outline.png")}},
	{"earth", {PROJECT_SOURCE_DIR + std::string("/assets/earth_bar.png"), PROJECT_SOURCE_DIR + std::string("/assets/earth_outline.png")}},
	{"wood", {PROJECT_SOURCE_DIR + std::string("/assets/wood_bar.png"), PROJECT_SOURCE_DIR + std::string("/assets/wood_outline.png")}},
	{"metal", {PROJECT_SOURCE_DIR + std::string("/assets/metal_bar.png"), PROJECT_SOURCE_DIR + std::string("/assets/metal_outline.png")}}
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
		else if (name == "magicback") {
			img = new Magic();
		}
		else {
			img = new UIImg();
		}

		img->Init(scWidth, scHeight, { startX, startY }, percent, aspect);
		img->SetTexture(GetTexture(name));

		if (name == "magicback") {
			Magic* ma = dynamic_cast<Magic*>(img);
			for (const auto& pair : MagicUI) {
				const std::string& elementName = pair.first;
				const std::string& path1 = std::get<0>(pair.second);
				const std::string& path2 = std::get<1>(pair.second);
				const std::string& bar = elementName + "bar";
				const std::string& border = elementName + "border";
				LoadTexture(bar, path1);
				LoadTexture(border, path2);

				MagicElement e;
				e.name = elementName;
				e.borderTexture = GetTexture(border);
				e.manaTexture = GetTexture(bar);
				ma->powers.push_back(e);

			}

			const float radius = 75;
			const float total = ma->powers.size();
			for (int i = 0; i < total; i++) {
				float angle = glm::radians(360.0f / total * i);
				float x = ma->centerX + cos(angle) * radius;
				float y = ma->centerY + sin(angle) * radius;
				ma->powers[i].position = glm::vec2(x, y);
				ma->powers[i].angleOffset = angle;
			}
		}


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

void UIManager::update(const OtherPlayerStats& p) {
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
