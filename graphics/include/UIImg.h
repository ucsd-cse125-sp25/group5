#pragma once

#include "core.h"
#include "Global.h"
#include <vector>
#include <iostream>

class UIImg {
public:
	virtual void Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio);
	virtual void Draw();
	virtual void Update(const PlayerStats& p);
};


class HealthBar : public UIImg {
public:
	void Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio) override;
	void Draw() override;
	void Update(const PlayerStats& p) override;
private:
	glm::vec3 healthColor;
	glm::vec3 containerColor;
	std::vector<float> health;
	std::vector<float> container;

	GLuint VAO[2], VBO[2], EBO;
};