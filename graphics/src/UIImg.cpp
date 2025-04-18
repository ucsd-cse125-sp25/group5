#include "UIImg.h"

void UIImg::Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio) {
	shaderProgram = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	projection = glm::ortho(0.0f, 1200.0f, 0.0f, 900.0f, -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	float offsetX = scWidth * percent;
	float offsetY = scHeight * percent * ratio;

	uiData = {
		//Position                                     //UV         //Color
		startPos[0], startPos[1],                      0.0f, 0.0f,  baseColor[0], baseColor[1], baseColor[2],
		startPos[0] + offsetX, startPos[1],            1.0f, 0.0f,  baseColor[0], baseColor[1], baseColor[2],
		startPos[0] + offsetX, startPos[1] + offsetY,  1.0f, 1.0f,  baseColor[0], baseColor[1], baseColor[2],
		startPos[0], startPos[1] + offsetY,            0.0f, 1.0f,  baseColor[0], baseColor[1], baseColor[2],
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, uiData.size() * sizeof(float), uiData.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);

}

void UIImg::Update(const PlayerStats& p) {

}

void UIImg::Draw() {
	glUseProgram(shaderProgram);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void UIImg::SetTexture(GLuint tex) {
	texture = tex;
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

	shaderProgram = LoadShaders("shaders/healthbar.vert", "shaders/healthbar.frag");
	projection = glm::ortho(0.0f, 1200.0f, 0.0f, 900.0f, -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	containerColor = { 0.2f, 0.2f, 1.0f };
	healthColor = { 1.0f, 1.0f, 1.0f };

	float offsetX = scWidth * percent;
	float offsetY = scHeight * percent * ratio;

	health = {
		//Position                                     //UV         //Color
		startPos[0], startPos[1],                      0.0f, 0.0f,  healthColor[0], healthColor[1], healthColor[2],
		startPos[0] + offsetX, startPos[1],            1.0f, 0.0f,  healthColor[0], healthColor[1], healthColor[2],
		startPos[0] + offsetX, startPos[1] + offsetY,  1.0f, 1.0f,  healthColor[0], healthColor[1], healthColor[2],
		startPos[0], startPos[1] + offsetY,            0.0f, 1.0f,  healthColor[0], healthColor[1], healthColor[2],
	};

	container = {
		//Position                                     //UV        //Color
		startPos[0], startPos[1],                      0.0f, 0.0f, containerColor[0], containerColor[1], containerColor[2],
		startPos[0] + offsetX, startPos[1],            1.0f, 0.0f, containerColor[0], containerColor[1], containerColor[2],
		startPos[0] + offsetX, startPos[1] + offsetY,  1.0f, 1.0f, containerColor[0], containerColor[1], containerColor[2],
		startPos[0], startPos[1] + offsetY,            0.0f, 1.0f, containerColor[0], containerColor[1], containerColor[2],
	};

	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, container.size() * sizeof(float), container.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, health.size() * sizeof(float), health.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);
}

void HealthBar::Update(const PlayerStats &p) {
	float leftX = container[0];
	float rightX = container[7];
	float width = rightX - leftX;
	float percentage = float(p.currHP) / float(p.maxHP);

	glUseProgram(shaderProgram);
	glUniform1f(glGetUniformLocation(shaderProgram, "healthPercent"), percentage);
}

void HealthBar::SetTexture(GLuint texture) {
	healthTexture = texture;
}

void HealthBar::Draw() {
	glUseProgram(shaderProgram);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, healthTexture);

	glDisable(GL_DEPTH_TEST);

	glUniform1i(glGetUniformLocation(shaderProgram, "health"), false);
	glBindVertexArray(VAO[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUniform1i(glGetUniformLocation(shaderProgram, "health"), true);
	glBindVertexArray(VAO[1]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}