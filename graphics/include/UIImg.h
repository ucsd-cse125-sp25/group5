#pragma once

#include "core.h"
#include "Global.h"
#include "Shader.h"
#include <vector>
#include <iostream>

class UIImg {
public:
	virtual void Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio);
	virtual void Draw();
	virtual void Update(const OtherPlayerStats& p);
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


class HealthBar : public UIImg {
public:
	void Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const OtherPlayerStats& p) override;
	void SetTexture(GLuint texture);

	std::string name;
private:
	glm::vec3 healthColor;
	glm::vec3 containerColor;
	std::vector<float> health;
	std::vector<float> container;
	GLuint healthTexture;
	GLuint shaderProgram;
	glm::mat4 projection;

	GLuint VAO[2], VBO[2], EBO;
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

class Magic : public UIImg {
public:
	void Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const OtherPlayerStats& p);
	void SetTexture(GLuint texture);
	void StartRotate(int anim);

	std::vector<MagicElement> powers;
	std::vector<float> baseAngles;
	std::string name;
	float centerX;
	float centerY;
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

	double animStart;
	bool animating;
};