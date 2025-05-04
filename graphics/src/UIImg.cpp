#include "UIImg.h"

void UIImg::Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio) {
	shaderProgram = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	projection = glm::ortho(0.0f, scWidth, 0.0f, scHeight, -1.0f, 1.0f);
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

void UIImg::Update(const OtherPlayerStats& p) {

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
	projection = glm::ortho(0.0f, scWidth, 0.0f, scHeight, -1.0f, 1.0f);
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

void HealthBar::Update(const OtherPlayerStats &p) {
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

void Magic::Init(float scWidth, float scHeight, std::vector<float> startPos, float percent, float ratio) {
	uiWidth = scWidth * percent;
	uiHeight = scHeight * percent * ratio;

	centerX = startPos[0] + (uiWidth / 2.0f) + 15;
	centerY = startPos[1] + (uiHeight / 2.0f) - 15;

	manaWidth = scWidth * 0.1;

	std::cout << "Creating the magic ui element" << std::endl;

	shaderProgram = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	manaProgram = LoadShaders("shaders/magic.vert", "shaders/magic.frag");
	projection = glm::ortho(0.0f, scWidth, 0.0f, scHeight, -1.0f, 1.0f);

	float offsetX = uiWidth;
	float offsetY = uiHeight;

	quad = {
		//Position                                     //UV         //Color
		startPos[0], startPos[1],                      0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		startPos[0] + offsetX, startPos[1],            1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		startPos[0] + offsetX, startPos[1] + offsetY,  1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
		startPos[0], startPos[1] + offsetY,            0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
	};

	mana = {
		//Position                     //UV         //Color
		0.0f, 0.0f,                    0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		manaWidth, 0.0f,               1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		manaWidth, manaWidth,         1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
		0.0f, manaWidth,              0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
	};

	//Buffers for the background
	glGenVertexArrays(1, &backVAO);
	glGenBuffers(1, &backVBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(backVAO);
	glBindBuffer(GL_ARRAY_BUFFER, backVBO);
	glBufferData(GL_ARRAY_BUFFER, quad.size() * sizeof(float), quad.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);


	//Buffers for the acutal elements
	glGenVertexArrays(1, &elemVAO);
	glGenBuffers(1, &elemVBO);

	glBindVertexArray(elemVAO);
	glBindBuffer(GL_ARRAY_BUFFER, elemVBO);
	glBufferData(GL_ARRAY_BUFFER, mana.size() * sizeof(float), mana.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);
}

void Magic::Update(const OtherPlayerStats& p) {
	//loop through each struct and update according to the player
	float waterPerc = float(p.currWater) / float(p.maxWater);
	float firePerc = float(p.currFire) / float(p.maxFire);
	float earthPerc = float(p.currEarth) / float(p.maxEarth);
	float woodPerc = float(p.currWood) / float(p.maxWood);
	float metalPerc = float(p.currMetal) / float(p.maxMetal);
	//std::cout << waterPerc << " " << firePerc << " " << earthPerc << " " << woodPerc << " " << metalPerc << std::endl;
	int size = powers.size();
	for (int i = 0; i < size; i++) {
		const std::string name = powers[i].name;
		if (name == "water") {
			powers[i].currMana = waterPerc;
		}
		else if (name == "fire") {
			powers[i].currMana = firePerc;
		}
		else if (name == "earth") {
			powers[i].currMana = earthPerc;
		}
		else if (name == "wood") {
			powers[i].currMana = woodPerc;
		}
		else if (name == "metal") {
			powers[i].currMana = metalPerc;
		}
	}

	double now = glfwGetTime();
	if (animating) {
		double elapsed = now - animStart;
		float t = elapsed / SPIN_DURATION;
		if (elapsed >= SPIN_DURATION) {
			animating = false;
			for (int i = 0; i < size; i++) {
				powers[i].currIdx = powers[i].targetIdx;
				float angle = baseAngles[powers[i].currIdx];
				float x = centerX + cos(angle) * MANA_RADIUS;
				float y = centerY + sin(angle) * MANA_RADIUS;
				powers[i].position = glm::vec2(x, y);
			}
		}
		else {
			for (int i = 0; i < size; i++) {
				int curr = powers[i].currIdx;
				int target = powers[i].targetIdx;
				float startAngle = baseAngles[curr];
				float endAngle = baseAngles[target];

				if (endAngle - startAngle > glm::pi<float>()) endAngle -= glm::two_pi<float>();
				else if (startAngle - endAngle > glm::pi<float>()) endAngle += glm::two_pi<float>();

				float nowAngle = glm::mix(startAngle, endAngle, t);
				float x = centerX + cos(nowAngle) * MANA_RADIUS;
				float y = centerY + sin(nowAngle) * MANA_RADIUS;
				powers[i].position = glm::vec2(x, y);
			}
		}
	}
}

void Magic::Draw() {
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, backTexture);

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(backVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//loop through each struct, send the uniform for mana % then draw
	float seconds = glfwGetTime();
	glUseProgram(manaProgram);
	glUniformMatrix4fv(glGetUniformLocation(manaProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniform1f(glGetUniformLocation(manaProgram, "time"), seconds);
	glBindVertexArray(elemVAO);
	for (const auto& p : powers) {
		float scale = (p.targetIdx == 0) ? 1.2f : 0.8f;
		//translate, scale then translate by the offset
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(p.position.x, p.position.y, 0.0f));
		model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
		model = glm::translate(model, glm::vec3(-manaWidth / 2.0f, -manaWidth / 2.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(manaProgram, "model"), 1, GL_FALSE, &model[0][0]);
		glUniform1f(glGetUniformLocation(manaProgram, "manaPercent"), p.currMana);
		glUniform1i(glGetUniformLocation(manaProgram, "isMana"), 1);
		int isSelected = (p.targetIdx == 0) ? 1 : 0;
		glUniform1i(glGetUniformLocation(manaProgram, "selectedMana"), isSelected);
		glBindTexture(GL_TEXTURE_2D, p.manaTexture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glUniform1i(glGetUniformLocation(manaProgram, "isMana"), 0);
		glBindTexture(GL_TEXTURE_2D, p.borderTexture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void Magic::SetTexture(GLuint texture) {
	backTexture = texture;
}

void Magic::StartRotate(int anim) {
	if (animating) {
		return;
	}
	//Calculate all the corresponding next indices
	for (auto& p : powers) {
		int size = baseAngles.size();
		if (anim == 0) {
			p.targetIdx = (p.targetIdx - 1 + size) % size;
		}
		else if (anim == 1) {
			p.targetIdx = (p.targetIdx + 1) % size;
		}
	}

	//Set bool flag
	animStart = glfwGetTime();
	animating = true;
	
}