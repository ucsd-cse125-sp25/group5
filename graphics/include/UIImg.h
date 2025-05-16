#pragma once

#include "core.h"
#include "Global.h"
#include "Shader.h"
#include <vector>
#include <iostream>

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


class HealthBar : public UIImg {
public:
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	void SetTexture(GLuint texture);
	void StartRegrow(int anim);

	std::string name;
private:
	std::vector<float> health;
	GLuint healthTexture;
	GLuint shaderProgram;
	glm::mat4 projection;

	GLuint VAO, VBO, EBO;
	float percent = 1.0; //0.0-1.0
	double animStart;
	bool animating;
	float animDuration = 3.0f;
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
	void Init(std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const UIData& p) override;
	void UpdateLayout();
	void SetTexture(GLuint texture);
	void StartRotate(int anim);

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