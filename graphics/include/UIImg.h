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