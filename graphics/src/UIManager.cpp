#include "UIManager.h"
#include <iostream>
#include "stb_image.h"
#include <map>
#include <glm/gtx/string_cast.hpp>

/**
* UIStorage is a map that holds the texture configurations for UI elements
* The data is a tuple containing:
* - path to the texture (std::string)
* - Game state it should appear during (GameState)
* - start position x (float) 5/4 - changed to percent of total width
* - start position y (float) 5/4 - changed to percent of total height
* - The width as a percentage of screen width (float)
* - The aspect ratio of the texture (float)
**/
static std::unordered_map<std::string, std::tuple<std::string, GameState, float, float, float, float>> UIStorage = {
	{ "magicback", { PROJECT_SOURCE_DIR + std::string("/assets/UIUIUI.png"), GameState::MATCH, 0.7, 0.0, 0.3, 1.0} },
<<<<<<< HEAD
	{ "reticle", {PROJECT_SOURCE_DIR + std::string("/assets/reticle.png"), GameState::MATCH, 0.5, 0.5, 0.05, 1.0}},
	{ "healthbar", {PROJECT_SOURCE_DIR + std::string("/assets/branch.png"), GameState::MATCH, 0.0, 0.0, 0.5, 0.5}}
=======
	{ "reticle", {PROJECT_SOURCE_DIR + std::string("/assets/reticle.png"), GameState::MATCH, 0.5, 0.5, 0.05, 1.0}}
>>>>>>> TEXTANDUI
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

static std::unordered_map<std::string, std::string> HealthUI = {
	{"b0", PROJECT_SOURCE_DIR + std::string("/assets/b0.png")},
	{"b1", PROJECT_SOURCE_DIR + std::string("/assets/b1.png")},
	{"b2", PROJECT_SOURCE_DIR + std::string("/assets/b2.png")},
	{"b3", PROJECT_SOURCE_DIR + std::string("/assets/b3.png")},
	{"b4", PROJECT_SOURCE_DIR + std::string("/assets/b4.png")},
	{"b5", PROJECT_SOURCE_DIR + std::string("/assets/b5.png")}
};


//0-9 + colon (:)
static std::unordered_map<std::string, std::string> Numbers = {
	{"0", PROJECT_SOURCE_DIR + std::string("/assets/numbers_0_-removebg-preview.png")},
	{"1", PROJECT_SOURCE_DIR + std::string("/assets/numbers_1_-removebg-preview.png")},
	{"2", PROJECT_SOURCE_DIR + std::string("/assets/numbers_2_-removebg-preview.png")},
	{"3", PROJECT_SOURCE_DIR + std::string("/assets/numbers_3_-removebg-preview.png")},
	{"4", PROJECT_SOURCE_DIR + std::string("/assets/numbers_4_-removebg-preview.png")},
	{"5", PROJECT_SOURCE_DIR + std::string("/assets/numbers_5_-removebg-preview.png")},
	{"6", PROJECT_SOURCE_DIR + std::string("/assets/numbers_6_-removebg-preview.png")},
	{"7", PROJECT_SOURCE_DIR + std::string("/assets/numbers_7_-removebg-preview.png")},
	{"8", PROJECT_SOURCE_DIR + std::string("/assets/numbers_8_-removebg-preview.png")},
	{"9", PROJECT_SOURCE_DIR + std::string("/assets/numbers_9_-removebg-preview.png")},
	{":", PROJECT_SOURCE_DIR + std::string("/assets/colom_1_-removebg-preview.png")},
};


static std::vector<std::string> MagicOrder{
	"metal", "wood", "water", "fire", "earth"
};

static std::vector<std::string> FlowerOrder{
	"b0", "b1", "b2", "b3", "b4", "b5"
};

//Loads textures and creates UI elements
void UIManager::Init() {

	//load in number textures to use
	for (const auto& pair : Numbers) {
		const std::string& name = pair.first;
		const std::string& path = pair.second;
		LoadTexture(name, path);
	}
	//add a clock to match elements
	//UIImg* clock = new Clock();
	UIImg* clock = new Clock();
	std::vector<float> startPerc = { 0.3, 0.9 };
	clock->Init(startPerc, 0.05, 1.0);
	matchElements.push_back(clock);
	Clock* c = dynamic_cast<Clock*>(clock);
	c->texs = &textures; //Mickey mouse
	

	for (const auto& pair : UIStorage) {
		const std::string& name = pair.first;
		const std::string& path = std::get<0>(pair.second);
		GameState state = std::get<1>(pair.second);
		float percX = std::get<2>(pair.second);
		float percY = std::get<3>(pair.second);
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

		img->Init({ percX, percY }, percent, aspect);
		img->SetTexture(GetTexture(name));

		if (name == "magicback") {
			Magic* ma = dynamic_cast<Magic*>(img);
			for (const auto& key : MagicOrder) {
				const auto& tuple = MagicUI[key];
				const std::string& elementName = key;
				const std::string& path1 = std::get<0>(tuple);
				const std::string& path2 = std::get<1>(tuple);
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

			const float total = ma->powers.size();
			const auto startAngle = glm::radians(144.0f);
			for (int i = 0; i < total; i++) {
				float angle = startAngle - glm::radians(360.0f / total * i);

				// Normalize to [0 2pi]
				if (angle < 0.0f) {
					angle += glm::two_pi<float>();
				}
				float radius = ma->manaRadius * WINDOWWIDTH;
				float x = ma->centerX + cos(angle) * radius;
				float y = ma->centerY + sin(angle) * radius;
				ma->powers[i].position = glm::vec2(x, y);
				ma->powers[i].currIdx = i;
				ma->powers[i].targetIdx = i;
				ma->baseAngles.push_back(angle);
			}

		}
		else if (name == "healthbar") {
			HealthBar* hb = dynamic_cast<HealthBar*>(img);
			int count = 0;
			for (const auto& key : FlowerOrder) {
				const std::string path = HealthUI[key];
				LoadTexture(key, path);
				FlowerElement f;
				f.name = key;
				f.flowerTex = GetTexture(key);
				f.decayed = false;
				f.animating = false;
				f.branchPos = { hb->flowerPositions[count].at(0), hb->flowerPositions[count].at(1) };
				f.currPos = f.branchPos;
				f.endPos = f.branchPos;
				f.endPos.y = f.endPos.y - 200.0f;
				f.decayPerc = (float)((count + 1) * 20) / (float)120;
				f.currScale = 1.0f;
				hb->flowers.push_back(f);
				count++;
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

void UIManager::update(const UIData& p) {
	switch (currState) {
	case GameState::LOBBY:
		for (auto* img : lobbyElements) {
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
		for (auto* img : lobbyElements) {
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

int UIManager::getPowerup() {
	
	return 0;
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

void UIManager::TriggerAnim(int anim) {
	for (auto* img : matchElements) {
		if (Magic* ma = dynamic_cast<Magic*>(img)) {
			if (anim == 0 || anim == 1) {
				ma->StartRotate(anim);
			}
		}
		else if (HealthBar* hb = dynamic_cast<HealthBar*>(img)) {
			if (anim == 2) {
				hb->StartRegrow(anim);
			}
		}
	}
}
