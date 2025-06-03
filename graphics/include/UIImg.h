#pragma once

#include "core.h"
#include "Global.h"
#include "Shader.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <unordered_map>

class UIImg {
public:
	virtual void Init(std::vector<float> startPos, float percent, float ratio);
	virtual void Draw();
	virtual void Update(const UIData& p);
	virtual void SetTexture(GLuint tex);

	std::string name;
private:
	glm::vec3 baseColor = { 1.0f, 1.0f, 1.0f }; // {1.0,1.0,1.0} means no change to texture color
	GLuint texture;
	GLuint shaderProgram;
	glm::mat4 projection;
	std::vector<float> uiData;

	GLuint VAO, VBO, EBO;
	
};

struct FlowerElement {
	std::string name;
	glm::vec2 branchPos;
	glm::vec2 currPos;
	glm::vec2 endPos;
	double startTime;
	GLuint flowerTex;
	float decayPerc;
	float currScale;
	bool decayed;
	bool animating;
	bool blooming;
};

class HealthBar : public UIImg {
public:
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	void SetTexture(GLuint texture);
	void StartRegrow();

	std::vector<std::vector<float>> flowerPositions;
	//std::unordered_map<std::string, GLuint>* texs;
	std::vector<FlowerElement> flowers;
	std::string name;
private:
	std::vector<float> health;
	std::vector<float> flower;
	
	std::vector<float> flowerStart;
	GLuint healthTexture;
	GLuint shaderProgram;
	glm::mat4 projection;

	GLuint VAO, VBO, EBO;
	GLuint FlowerVAO, FlowerVBO;
	float percent = 1.0; //0.0-1.0
	int lastHealth = 120;
	bool isAlive = true;
	float flowerWidth;
	double animStart;
	bool animating;
	float animDuration = 2.0f;
	float decayDuration = 1.3f;
	float bloomDuration = 0.9f;
};

class ToolTips : public UIImg {
public:
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	std::string name;
	std::unordered_map<std::string, GLuint>* texs;
	ClientGame* client;
private:
	GLuint sprites[5];
	GLuint shaderProgram;
	glm::mat4 projection;
	std::vector<float> container;
	std::vector<float> bottomleft;
	GLuint VAO, VBO, EBO;
	GLuint VAOb, VBOb;
	bool draw = false;
	bool drawB = false;

};

struct MagicElement {
	std::string name;
	glm::vec2 position;
	int currIdx;
	int targetIdx;
	float currMana;
	GLuint borderTexture;
	GLuint manaTexture;
};

class Clock : public UIImg {
public:
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	std::string name;
	std::unordered_map<std::string, GLuint>* texs;
	ClientGame* client;
private:
	GLuint digits[5];
	GLuint shaderProgram;
	glm::mat4 projection;
	std::vector<float> container;
	double start = 0.0;
	int seconds;
	float widthPercSpacing = 0.035;

	GLuint VAO, VBO, EBO;
};

class HealthNums : public UIImg {
public:
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	std::string name;
	std::unordered_map<std::string, GLuint>* texs;
private:
	GLuint digits[4];
	GLuint shaderProgram;
	glm::mat4 projection;
	std::vector<float> container;
	int health;
	float widthPercSpacing = 0.035;

	GLuint VAO, VBO, EBO;
};

class Characters : public UIImg {
public:
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	std::string name;
	std::unordered_map<std::string, GLuint>* texs;
	ClientGame* client;
private:
	GLuint sprites[4];
	GLuint shaderProgram;
	glm::mat4 projection;
	std::vector<float> container;
	double start = 0.0;
	int players;

	GLuint VAO, VBO, EBO;
};

class Killfeed : public UIImg {
public:
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	std::string name;
	std::unordered_map<std::string, GLuint>* texs;
private:
	GLuint players[4];
	GLuint action[4];
	GLuint shaderProgram;
	glm::mat4 projection;
	std::vector<float> container;
	double start = 0.0;
	UIData uidata;

	GLuint VAO, VBO, EBO;
};

class Magic : public UIImg {
public:
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	void UpdateLayout();
	void SetTexture(GLuint texture);
	void StartRotate(int anim);
	void SetRotate(int anim);

	std::vector<MagicElement> powers;
	std::vector<float> baseAngles;
	std::string name;
	float percX;
	float percY;
	float centerX = 0.0f;
	float centerY = 0.0f;
	float manaRadius = 0.06f;
	int scWidth; int scHeight;
private:
	std::vector<float> quad;
	std::vector<float> mana;
	GLuint shaderProgram;
	GLuint manaProgram;
	glm::mat4 projection;

	GLuint backTexture;

	GLuint backVAO, backVBO;
	GLuint elemVAO, elemVBO;
	GLuint EBO;

	float uiWidth;
	float uiHeight;
	float manaWidth;
	std::vector<float> position;
	float percent;
	float ratio;

	double animStart;
	bool animating;
	float spinDuration = 0.1f;

};

class Vignette : public UIImg {
public:
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	void SetTexture(GLuint tex) override;
	std::string name;
	ClientGame* client;
private: 
	GLuint texture;
	GLuint goldTexture;
	GLuint shaderProgram;
	glm::mat4 projection;
	std::vector<float> uiData;
	int lastHealth;
	bool isAlive;
	bool isLow;
	bool hasFlag;
	double animStart;
	float damageDuration = 0.1f;
	GLuint VAO, VBO, EBO;
};