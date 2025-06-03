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
static std::unordered_map<std::string, std::tuple<std::string, GamePhase, float, float, float, float>> UIStorage = {
	{ "magicback", { PROJECT_SOURCE_DIR + std::string("/assets/UIUIUI.png"), GamePhase::IN_GAME, 0.7, 0.0, 0.3, 1.0} },
	{ "reticle", {PROJECT_SOURCE_DIR + std::string("/assets/reticle.png"), GamePhase::IN_GAME, 0.5, 0.5, 0.05, 1.0}},
	{ "healthbar", {PROJECT_SOURCE_DIR + std::string("/assets/branch.png"), GamePhase::IN_GAME, 0.0, 0.0, 0.5, 0.5}},
	{ "gameTitle", {PROJECT_SOURCE_DIR + std::string("/assets/adoptme.png"), GamePhase::WAITING, 0.5, 0.75, 0.35, 1.0}},
	{ "loading1", {PROJECT_SOURCE_DIR + std::string("/assets/sdfh-removebg-preview.png"), GamePhase::WAITING, 0.2, 0.2, 0.1, 1.0}},
	{ "loading2", {PROJECT_SOURCE_DIR + std::string("/assets/sdfh-removebg-preview.png"), GamePhase::WAITING, 0.4, 0.2, 0.1, 1.0}},
	{ "loading3", {PROJECT_SOURCE_DIR + std::string("/assets/sdfh-removebg-preview.png"), GamePhase::WAITING, 0.6, 0.2, 0.1, 1.0}},
	{ "loading4", {PROJECT_SOURCE_DIR + std::string("/assets/sdfh-removebg-preview.png"), GamePhase::WAITING, 0.8, 0.2, 0.1, 1.0}},
  { "vignette", {PROJECT_SOURCE_DIR + std::string("/assets/vignette.png"), GamePhase::IN_GAME, 0.0, 0.0, 1.0, 1.0}},
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


static std::unordered_map<std::string, std::string> ToolTipsTexs = {
	{ "tooltip1", PROJECT_SOURCE_DIR + std::string("/assets/tooltip1.jpg")},
	{ "tooltip2", PROJECT_SOURCE_DIR + std::string("/assets/tooltip2.jpg")},
	{ "tooltip3", PROJECT_SOURCE_DIR + std::string("/assets/tooltip3.jpg")} ,
	{ "tooltip4", PROJECT_SOURCE_DIR + std::string("/assets/tooltip4.jpg")} ,
	{ "tooltip5", PROJECT_SOURCE_DIR + std::string("/assets/tooltip5.jpg")}
};

static std::unordered_map<std::string, std::string> LobbyCharacters = {
	{ "character1", PROJECT_SOURCE_DIR + std::string("/assets/character1.png")},
	{ "character2", PROJECT_SOURCE_DIR + std::string("/assets/character2.png")},
	{ "character3", PROJECT_SOURCE_DIR + std::string("/assets/character3.png")} ,
	{ "character4", PROJECT_SOURCE_DIR + std::string("/assets/character4.png")}
};

static std::unordered_map<std::string, std::string> KillfeedSprites = {
	{ "player0", PROJECT_SOURCE_DIR + std::string("/assets/numbers_1.png")},
	{ "player1", PROJECT_SOURCE_DIR + std::string("/assets/numbers_2.png")},
	{ "player2", PROJECT_SOURCE_DIR + std::string("/assets/numbers_3.png")} ,
	{ "player3", PROJECT_SOURCE_DIR + std::string("/assets/numbers_4.png")},
	{ "action0", PROJECT_SOURCE_DIR + std::string("/assets/action1.png")},
	{ "action1", PROJECT_SOURCE_DIR + std::string("/assets/action2.png")},
	{ "action2", PROJECT_SOURCE_DIR + std::string("/assets/action3.png")} ,
	{ "action3", PROJECT_SOURCE_DIR + std::string("/assets/action4.png")}
};

//0-9 + colon (:)
static std::unordered_map<std::string, std::string> Numbers = {
	{"0", PROJECT_SOURCE_DIR + std::string("/assets/numbers_0.png")},
	{"1", PROJECT_SOURCE_DIR + std::string("/assets/numbers_1.png")},
	{"2", PROJECT_SOURCE_DIR + std::string("/assets/numbers_2.png")},
	{"3", PROJECT_SOURCE_DIR + std::string("/assets/numbers_3.png")},
	{"4", PROJECT_SOURCE_DIR + std::string("/assets/numbers_4.png")},
	{"5", PROJECT_SOURCE_DIR + std::string("/assets/numbers_5.png")},
	{"6", PROJECT_SOURCE_DIR + std::string("/assets/numbers_6.png")},
	{"7", PROJECT_SOURCE_DIR + std::string("/assets/numbers_7.png")},
	{"8", PROJECT_SOURCE_DIR + std::string("/assets/numbers_8.png")},
	{"9", PROJECT_SOURCE_DIR + std::string("/assets/numbers_9.png")},
	{":", PROJECT_SOURCE_DIR + std::string("/assets/colon.png")},
};


static std::vector<std::string> MagicOrder{
	"metal", "wood", "water", "fire", "earth"
};

static std::vector<std::string> FlowerOrder{
	"b0", "b1", "b2", "b3", "b4", "b5"
};

//Loads textures and creates UI elements
void UIManager::Init(ClientGame* client) {
	this->client = client;

	//load in number textures to use
	for (const auto& pair : Numbers) {
		const std::string& name = pair.first;
		const std::string& path = pair.second;
		LoadTexture(name, path);
	}
	//add a clock to match elements
	//UIImg* clock = new Clock();
	UIImg* clock = new Clock();

	std::vector<float> startPerc = { 0.0, 0.9 };
	clock->Init(startPerc, 0.07, 1.0);
	//lobbyElements.push_back(clock);
	countdownElements.push_back(clock);
	matchElements.push_back(clock);
	Clock* cl = dynamic_cast<Clock*>(clock);
	cl->texs = &textures; //Mickey mouse
	cl->client = client; //Mickey mouse

	//add a clock to match elements
	//UIImg* clock = new Clock();
	UIImg* healthNums = new HealthNums();
	std::vector<float> startPercHealthNums = { 0.1, 0.965 };
	healthNums->Init(startPercHealthNums, 0.04, 1.0);
	matchElements.push_back(healthNums);
	HealthNums* hn = dynamic_cast<HealthNums*>(healthNums);
	hn->texs = &textures; //Mickey mouse
	
	for (const auto& pair : LobbyCharacters) {
		const std::string& name = pair.first;
		const std::string& path = pair.second;
		LoadTexture(name, path);
	}
	UIImg* characters = new Characters();
	std::vector<float> startPercchar = { 0.2, 0.2 };
	
	Characters* ch = dynamic_cast<Characters*>(characters);
	ch->texs = &textures; //Mickey mouse
	characters->Init(startPercchar, 0.14, 1.0);

	ch->client = client;

	for (const auto& pair : ToolTipsTexs) {
		const std::string& name = pair.first;
		const std::string& path = pair.second;
		LoadTexture(name, path);
	}

	UIImg* tooltips = new ToolTips();
	std::vector<float> startPerctool = { 0.2, 0.35 };

	ToolTips* tt = dynamic_cast<ToolTips*>(tooltips);
	tt->texs = &textures; //Mickey mouse
	tooltips->Init(startPerctool, 0.12, 1.8);

	tt->client = client;


	for (const auto& pair : KillfeedSprites) {
		const std::string& name = pair.first;
		const std::string& path = pair.second;
		LoadTexture(name, path);
	}
	UIImg* killfeed = new Killfeed();
	std::vector<float> startPercKill = { 0.75, 0.94 };

	Killfeed* kf = dynamic_cast<Killfeed*>(killfeed);
	kf->texs = &textures; //Mickey mouse
	killfeed->Init(startPercKill, 0.09, 1.0);

	for (const auto& pair : UIStorage) {
		const std::string& name = pair.first;
		const std::string& path = std::get<0>(pair.second);
		GamePhase state = std::get<1>(pair.second);
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
		else if (name == "vignette") {
			Vignette *vig = new Vignette();
			vig->client = client;
			img = vig;
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
		case GamePhase::WAITING:
			lobbyElements.push_back(img);

			if (name != "gameTitle") {
				countdownElements.push_back(img);
			}
			break;
		case GamePhase::IN_GAME:
			matchElements.push_back(img);
			break;
		}
		
	}

	lobbyElements.push_back(characters);
	countdownElements.push_back(characters);
	matchElements.push_back(killfeed);
	lobbyElements.push_back(tooltips);
	matchElements.push_back(tooltips);
}

void UIManager::update(const UIData& p) {
	switch (client->GameState.phase) {
	case GamePhase::WAITING:
		for (auto* img : lobbyElements) {
			img->Update(p);
		}
		break;
	case GamePhase::PRE_GAME:
		for (auto* img : countdownElements) {
			img->Update(p);
		}
		break;
	case GamePhase::IN_GAME:
		for (auto* img : matchElements) {
			img->Update(p);
		}
		break;
	}
}

void UIManager::draw() {
	switch (client->GameState.phase) {
	case GamePhase::WAITING:
		for (auto* img : lobbyElements) {
			img->Draw();
		}
		break;
	case GamePhase::PRE_GAME:
		for (auto* img : countdownElements) {
			img->Draw();
		}
		break;
	case GamePhase::IN_GAME:
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
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

void UIManager::NextGamePhase() {

}

GamePhase UIManager::GetGamePhase() {
	return client->GameState.phase;
}

void UIManager::TriggerAnim(int anim) {
	for (auto* img : matchElements) {
		if (Magic* ma = dynamic_cast<Magic*>(img)) {
			if (anim == 0 || anim == 1) {
				ma->StartRotate(anim);
			}
			else if (anim >= 3 && anim <= 7) {
				ma->SetRotate(anim);
			}
		}
		else if (HealthBar* hb = dynamic_cast<HealthBar*>(img)) {
			if (anim == 2) {
				hb->StartRegrow();
			}
		}
	}
}
