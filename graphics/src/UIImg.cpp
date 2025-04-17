#include "UIImg.h"

void UIImg::Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio) {

}

void UIImg::Update(const PlayerStats& p) {

}

void UIImg::Draw() {

}



/**
* @brief Initalizes the quads of both the health and container bars.
* Using pixel coordinates: (0,0) will be bottom left of the screen
*
* @param scWidth: Width of the window
* @param scHeight: Height of the window
* @param startPos: Starting coordinate of quad
* @param percent: Percentage of the window width the UI element should occupy
* @param ratio: Aspect ratio of the UI element
**/
void HealthBar::Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio) {
	containerColor = { 1.0f, 1.0f, 1.0f };
	healthColor = { 0.3f, 0.0f, 0.0f };

	float offsetX = scWidth * percent;
	float offsetY = scHeight * percent * ratio;

	health = {
		//Position                                       //Color
		startPos[0], startPos[1],                        healthColor[0], healthColor[1], healthColor[2],
		startPos[0] + offsetX, startPos[1],              healthColor[0], healthColor[1], healthColor[2],
		startPos[0] + offsetX, startPos[1] + offsetY,    healthColor[0], healthColor[1], healthColor[2],
		startPos[0], startPos[1] + offsetY,              healthColor[0], healthColor[1], healthColor[2],
	};

	container = {
		//Position                                       //Color
		startPos[0], startPos[1],                        containerColor[0], containerColor[1], containerColor[2],
		startPos[0] + offsetX, startPos[1],              containerColor[0], containerColor[1], containerColor[2],
		startPos[0] + offsetX, startPos[1] + offsetY,    containerColor[0], containerColor[1], containerColor[2],
		startPos[0], startPos[1] + offsetY,              containerColor[0], containerColor[1], containerColor[2],
	};

	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glBufferData(GL_ARRAY_BUFFER, container.size() * sizeof(float), container.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glBufferData(GL_ARRAY_BUFFER, health.size() * sizeof(float), health.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);
}

void HealthBar::Update(const PlayerStats &p) {
	float percentage = float(p.currHP) / float(p.maxHP);
	health[5] = container[5] * percentage;
	health[10] = container[10] * percentage;

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

	glBufferSubData(GL_ARRAY_BUFFER, 1 * 5 * sizeof(float), 5 * sizeof(float), &health[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 2 * 5 * sizeof(float), 5 * sizeof(float), &health[10]);

	glBindVertexArray(0);

}

void HealthBar::Draw() {
	//std::cout << "Drawing the Healthbar!" << std::endl;
	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(VAO[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(VAO[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}