#include "UIImg.h"
#include "Global.h"
//#include "Scene.h"
//#include "network/ClientGame.h"
//#include "UIManager.h"

void UIImg::Init(std::vector<float> startPerc, float percent, float ratio) {
	shaderProgram = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	float uiWidth = WINDOWWIDTH * percent;
	float uiHeight = uiWidth * ratio;
	std::vector<float> startPos = {
		WINDOWWIDTH * startPerc[0],
		WINDOWHEIGHT * startPerc[1]
	};

	uiData = {
		//Position                                     //UV         //Color
		startPos[0] - (uiWidth/2), startPos[1] - (uiHeight/2),                      0.0f, 0.0f,  baseColor[0], baseColor[1], baseColor[2],
		startPos[0] + (uiWidth / 2), startPos[1] - (uiHeight / 2),            1.0f, 0.0f,  baseColor[0], baseColor[1], baseColor[2],
		startPos[0] + (uiWidth / 2), startPos[1] + (uiHeight / 2),  1.0f, 1.0f,  baseColor[0], baseColor[1], baseColor[2],
		startPos[0] - (uiWidth / 2), startPos[1] + (uiHeight / 2),            0.0f, 1.0f,  baseColor[0], baseColor[1], baseColor[2],
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

void UIImg::Update(const UIData& p) {

}

void UIImg::Draw() {
	glUseProgram(shaderProgram);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO);

	glm::mat4 model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void UIImg::SetTexture(GLuint tex) {
	texture = tex;
}

void Clock::Init(std::vector<float> startPerc, float percent, float ratio) {
	start = glfwGetTime();

	shaderProgram = LoadShaders("shaders/clock.vert", "shaders/clock.frag");
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);


	float uiWidth = WINDOWWIDTH * percent;
	float uiHeight = WINDOWHEIGHT * percent * ratio;
	float startX = WINDOWWIDTH * startPerc[0];
	float startY = WINDOWHEIGHT * startPerc[1];
	std::vector<float> startPos = { startX, startY };

	container = {
		//Position												   //UV         //Color
		startPos[0] - (uiWidth / 2), startPos[1] - (uiHeight / 2), 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] - (uiHeight / 2), 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] + (uiHeight / 2), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] - (uiWidth / 2), startPos[1] + (uiHeight / 2), 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, container.size() * sizeof(float), container.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);
}

void Clock::Update(const UIData& p) {
	if (p.seconds >= 0) {
		seconds = p.seconds;
	}

	int tempSeconds = seconds;


	//int seconds = (int)p.seconds;
	int tensMin = tempSeconds / (60 * 10);
	tempSeconds -= tensMin * (60 * 10);
	int onesMin = (tempSeconds / 60 ) % 10;
	tempSeconds -= onesMin * (60);
	int tensSec = (tempSeconds / 10) % 10;
	int onesSec = tempSeconds % 10;


	digits[0] = (*texs)[std::to_string(tensMin)];
	digits[1] = (*texs)[std::to_string(onesMin)];
	digits[2] = (*texs)[":"];
	digits[3] = (*texs)[std::to_string(tensSec)];
	digits[4] = (*texs)[std::to_string(onesSec)];
}

void Clock::Draw() {
	//seconds = timerStart - (glfwGetTime() - start);
	if (client->GameState.phase == IN_GAME && seconds < 1 * 30) {
		return;
	}

	glUseProgram(shaderProgram);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, healthTexture);

	glDisable(GL_DEPTH_TEST); 


	glBindVertexArray(VAO);
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(WINDOWWIDTH * 0.5, WINDOWHEIGHT* 0.04f, 0.0f));

	model = glm::translate(model, glm::vec3(WINDOWWIDTH * widthPercSpacing * -2.0f, 0.0f, 0.0f));
	for (GLuint num : digits) {
		//translate, scale then translate by the offset
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
		glBindTexture(GL_TEXTURE_2D, num);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		model = glm::translate(model, glm::vec3(WINDOWWIDTH * widthPercSpacing, 0.0f, 0.0f));
	}
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}


void HealthNums::Init(std::vector<float> startPerc, float percent, float ratio) {

	shaderProgram = LoadShaders("shaders/healthnums.vert", "shaders/healthnums.frag");
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);


	float uiWidth = WINDOWWIDTH * percent;
	float uiHeight = WINDOWHEIGHT * percent * ratio;
	float startX = WINDOWWIDTH * startPerc[0];
	float startY = WINDOWHEIGHT * startPerc[1];
	std::vector<float> startPos = { startX, startY };

	container = {
		//Position												   //UV         //Color
		startPos[0] - (uiWidth / 2), startPos[1] - (uiHeight / 2), 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] - (uiHeight / 2), 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] + (uiHeight / 2), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] - (uiWidth / 2), startPos[1] + (uiHeight / 2), 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, container.size() * sizeof(float), container.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);
}

void HealthNums::Update(const UIData& p) {
	health = p.currHP;

	int tempHealth = health;


	//int seconds = (int)p.seconds;
	int hundreds = (tempHealth / 100) % 10;
	int tens = (tempHealth / 10) % 10;
	int ones = tempHealth % 10;


	digits[0] = (*texs)["heart"];
	digits[1] = (*texs)[std::to_string(hundreds)];
	digits[2] = (*texs)[std::to_string(tens)];
	digits[3] = (*texs)[std::to_string(ones)];
}

void HealthNums::Draw() {


	glUseProgram(shaderProgram);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(VAO);
	glm::mat4 model = glm::mat4(1.0f);

	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

	for (GLuint num : digits) {
		//translate, scale then translate by the offset
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
		glBindTexture(GL_TEXTURE_2D, num);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		model = glm::translate(model, glm::vec3(WINDOWWIDTH * widthPercSpacing, 0.0f, 0.0f));
	}
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}



void ToolTips::Init(std::vector<float> startPerc, float percent, float ratio) {

	shaderProgram = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	float uiWidth = WINDOWWIDTH * percent;
	float uiHeight = WINDOWHEIGHT * percent * ratio;
	float startX = WINDOWWIDTH * startPerc[0];
	float startY = WINDOWHEIGHT * startPerc[1];
	std::vector<float> startPos = { startX, startY };

	container = {
		//Position												   //UV         //Color
		startPos[0] - (uiWidth / 2), startPos[1] - (uiHeight / 2), 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] - (uiHeight / 2), 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] + (uiHeight / 2), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] - (uiWidth / 2), startPos[1] + (uiHeight / 2), 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	float mini = WINDOWWIDTH * 0.05;
	float miniY = WINDOWHEIGHT * 0.05 * ratio;
	bottomleft = {
		0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		mini, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		mini, miniY, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, miniY, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, container.size() * sizeof(float), container.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);

	glGenVertexArrays(1, &VAOb);
	glGenBuffers(1, &VBOb);

	glBindVertexArray(VAOb);
	glBindBuffer(GL_ARRAY_BUFFER, VBOb);
	glBufferData(GL_ARRAY_BUFFER, bottomleft.size() * sizeof(float), bottomleft.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);


	sprites[0] = (*texs)["tooltip1"];
	sprites[1] = (*texs)["tooltip2"];
	sprites[2] = (*texs)["tooltip3"];
	sprites[3] = (*texs)["tooltip4"];
	sprites[4] = (*texs)["tooltip5"];
}

void ToolTips::Update(const UIData& p) {
	if (p.tooltip && client->GameState.phase == GamePhase::IN_GAME) {
		drawB = true;
		draw = false;
	}
	else if (p.currHP <= 0 && client->GameState.phase == GamePhase::IN_GAME) {
		draw = true;
		drawB = false;
	}
	else {
		drawB = false;
		draw = false;
	}
	//draw = p.tooltip || (p.currHP <= 0 && client->GameState.phase == GamePhase::IN_GAME);
}

void ToolTips::Draw() {
	//seconds = timerStart - (glfwGetTime() - start);
	if (!draw && !drawB) { return; }
	glUseProgram(shaderProgram);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glm::mat4 model = glm::mat4(1.0f);
	if (draw) {
		glBindVertexArray(VAO);
		for (int i = 0; i < 5; i++) {
			//translate, scale then translate by the offset
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
			model = glm::translate(model, glm::vec3(WINDOWWIDTH * 0.15, 0.0f, 0.0f));
			glBindTexture(GL_TEXTURE_2D, sprites[i]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	else if (drawB) {
		glBindVertexArray(VAOb);
		for (int i = 0; i < 5; i++) {
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
			model = glm::translate(model, glm::vec3(WINDOWWIDTH * 0.06, 0.0f, 0.0f));
			glBindTexture(GL_TEXTURE_2D, sprites[i]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, healthTexture);
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}


void Characters::Init(std::vector<float> startPerc, float percent, float ratio) {
	start = glfwGetTime();


	shaderProgram = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	float uiWidth = WINDOWWIDTH * percent;
	float uiHeight = WINDOWHEIGHT * percent * ratio;
	float startX = WINDOWWIDTH * startPerc[0];
	float startY = WINDOWHEIGHT * startPerc[1];
	std::vector<float> startPos = { startX, startY };

	container = {
		//Position												   //UV         //Color
		startPos[0] - (uiWidth / 2), startPos[1] - (uiHeight / 2), 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] - (uiHeight / 2), 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] + (uiHeight / 2), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] - (uiWidth / 2), startPos[1] + (uiHeight / 2), 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, container.size() * sizeof(float), container.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);

	sprites[0] = (*texs)["character1"];
	sprites[1] = (*texs)["character2"];
	sprites[2] = (*texs)["character3"];
	sprites[3] = (*texs)["character4"];
}

void Characters::Update(const UIData& p) {
	this->players = client->GameState.num_players;
}

void Characters::Draw() {
	//seconds = timerStart - (glfwGetTime() - start);

	glUseProgram(shaderProgram);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, healthTexture);

	glDisable(GL_DEPTH_TEST);


	glBindVertexArray(VAO);
	glm::mat4 model = glm::mat4(1.0f);
	for (int i = 0; i < players; i++) {
		//translate, scale then translate by the offset
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
		model = glm::translate(model, glm::vec3(WINDOWWIDTH * 0.2, 0.0f, 0.0f));
		glBindTexture(GL_TEXTURE_2D, sprites[i]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void Killfeed::Init(std::vector<float> startPerc, float percent, float ratio) {


	shaderProgram = LoadShaders("shaders/killfeed.vert", "shaders/killfeed.frag");
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	float uiWidth = WINDOWWIDTH * percent;
	float uiHeight = WINDOWHEIGHT * percent * ratio;
	float startX = WINDOWWIDTH * startPerc[0];
	float startY = WINDOWHEIGHT * startPerc[1];
	std::vector<float> startPos = { startX, startY };

	container = {
		//Position												   //UV         //Color
		startPos[0] - (uiWidth / 2), startPos[1] - (uiHeight / 2), 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] - (uiHeight / 2), 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + (uiWidth / 2), startPos[1] + (uiHeight / 2), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] - (uiWidth / 2), startPos[1] + (uiHeight / 2), 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, container.size() * sizeof(float), container.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);

	players[0] = (*texs)["character1"];
	players[1] = (*texs)["character2"];
	players[2] = (*texs)["character3"];
	players[3] = (*texs)["character4"];

	action[0] = (*texs)["character1"];
}

void Killfeed::Update(const UIData& p) {
	this->uidata = p;
}

void Killfeed::Draw() {
	//seconds = timerStart - (glfwGetTime() - start);

	glUseProgram(shaderProgram);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, healthTexture);

	glDisable(GL_DEPTH_TEST);


	glBindVertexArray(VAO);
	glm::mat4 baseModel = glm::mat4(1.0f);
	glm::mat4 lineStart = baseModel;
	glm::mat4 model = baseModel;
	for (int i = 0; i < KILLFEED_LENGTH; i++) {
		//printf("UIData at %d = %d %d %d %f\n",i, uidata.killfeed[i].attacker, uidata.killfeed[i].victim, uidata.killfeed[i].type, uidata.killfeed[i].lifetime);

		//what units is lifetime in?? 1/3 of a second???
		if (uidata.killfeed[i].attacker == uidata.killfeed[i].victim || uidata.killfeed[i].lifetime >= 30.0f) {
			continue;
		}
		float transparency = (30 - uidata.killfeed[i].lifetime)/1.5;

		glUniform1f(glGetUniformLocation(shaderProgram, "transparency"),  transparency);
		//attacker draw
		GLuint sprite = (*texs)["player" + std::to_string(uidata.killfeed[i].attacker)];
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
		glBindTexture(GL_TEXTURE_2D, sprite);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		model = glm::translate(model, glm::vec3(WINDOWWIDTH * 0.1f, 0.0f, 0.0f));
		//action draw
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
		glBindTexture(GL_TEXTURE_2D, (*texs)["action" + std::to_string(uidata.killfeed[i].type)]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		model = glm::translate(model, glm::vec3(WINDOWWIDTH * 0.1f, 0.0f, 0.0f));
		// will be -1 if there is no victim
		if (uidata.killfeed[i].victim != -1) {
			//victim draw only if there is a victim
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
			glBindTexture(GL_TEXTURE_2D, (*texs)["player" + std::to_string(uidata.killfeed[i].victim)]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		//newline of sprites
		model = glm::translate(lineStart, glm::vec3(0.0f, -WINDOWHEIGHT * 0.1f, 0.0f));
		lineStart = model;
		
	}
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}
/**
* @brief Initalizes the quads of both the health and container bars.
* Using pixel coordinates: (0,0) will be bottom left of the screen
*
* @param startPos: Starting coordinate of quad
* @param percent: Percentage of the window width the UI element should occupy
* @param ratio: Aspect ratio of the UI element
**/
void HealthBar::Init(std::vector<float> startPerc, float percent, float ratio) {

	shaderProgram = LoadShaders("shaders/healthbar.vert", "shaders/healthbar.frag");
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	float bWidth = WINDOWWIDTH * percent;
	float bHeight = WINDOWHEIGHT * percent * (806.0f / 1576.0f);
	flowerWidth = WINDOWWIDTH * percent * 0.15;

	float bump = (WINDOWHEIGHT * 0.8);
	std::vector<float> startPos = { WINDOWWIDTH * startPerc[0], WINDOWHEIGHT * startPerc[1] + bump};

	health = {
		//Position                                     //UV         //Color
		startPos[0], startPos[1],                      0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		startPos[0] + bWidth, startPos[1],            1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		startPos[0] + bWidth, startPos[1] + bHeight,  1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
		startPos[0], startPos[1] + bHeight,            0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
	};

	flowerStart = { 0.0f, 0.0f };
	flower = {
		flowerStart[0], flowerStart[1],                                 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		flowerStart[0] + flowerWidth, flowerStart[1],                   1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		flowerStart[0] + flowerWidth, flowerStart[1] + flowerWidth,     1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
		flowerStart[0], flowerStart[1] + flowerWidth,                   0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
	};

	flowerPositions = { {bWidth * 0.15f, bHeight * 0.09f + bump}, {bWidth * 0.35f, bHeight * 0.09f + bump}, {bWidth * 0.55f, bHeight * 0.3f + bump}, {bWidth * 0.7f, bHeight * 0.14f + bump}, {bWidth * 0.8f, bHeight * 0.78f + bump}, {bWidth * 0.92f, bHeight * 0.5f + bump}};
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	GLuint indices[] = { 0, 1, 2, 0, 2, 3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, health.size() * sizeof(float), health.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);

	glGenVertexArrays(1, &FlowerVAO);
	glGenBuffers(1, &FlowerVBO);


	glBindVertexArray(FlowerVAO);
	glBindBuffer(GL_ARRAY_BUFFER, FlowerVBO);
	glBufferData(GL_ARRAY_BUFFER, flower.size() * sizeof(float), flower.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);
}

void HealthBar::Update(const UIData &p) {

	float healthP = (float)p.currHP / (float)p.maxHP;

	double now = glfwGetTime();
	if (lastHealth <= 0 && p.currHP > lastHealth) {
		this->StartRegrow();
	}
	lastHealth = p.currHP;
	(lastHealth <= 0) ? isAlive = false : isAlive = true;


	if (animating) {
		double elapsed = now - animStart;
		float t = elapsed / animDuration;
		if (elapsed >= animDuration) {
			animating = false;
			percent = 1.0;
			for (int i = 0; i < flowers.size(); i++) {
				if (flowers[i].decayed) {
					flowers[i].blooming = true;
					flowers[i].startTime = glfwGetTime();
				}
			}
		}
		else {
			percent = t;
			for (int i = 0; i < flowers.size(); i++) {
				if (percent >= flowers[i].decayPerc && flowers[i].decayed && !flowers[i].blooming) {
					flowers[i].blooming = true;
					flowers[i].decayed = false;
					flowers[i].startTime = glfwGetTime();
				}
			}
		}
	}


	//This is to trigger the animation
	for (int i = 0; i < flowers.size(); i++) {
		if (!flowers[i].decayed && healthP < flowers[i].decayPerc && !flowers[i].animating && !animating) {
			//std::cout << "Trigger anim for: " << flowers[i].name << std::endl;
			flowers[i].startTime = glfwGetTime();
			flowers[i].animating = true;
		}
		else if (flowers[i].decayed && healthP >= flowers[i].decayPerc && !flowers[i].blooming && !animating) {
			std::cout << "Going to bloom flower: " << i << std::endl;
			flowers[i].currScale = 0.0;
			flowers[i].startTime = glfwGetTime();
			flowers[i].blooming = true;
		}
	}

	//This is to play the animation
	for (int i = 0; i < flowers.size(); i++) {
		if (flowers[i].animating) {
			double now = glfwGetTime();
			double elapsed = now - flowers[i].startTime;
			float t = elapsed / decayDuration;
			if (elapsed >= decayDuration) {
				flowers[i].animating = false;
				flowers[i].decayed = true;
				flowers[i].currPos = flowers[i].endPos;
				flowers[i].currPos = flowers[i].branchPos;
				flowers[i].currScale = 0.0;
			}
			else {
				flowers[i].currPos.y = glm::mix(flowers[i].branchPos.y, flowers[i].endPos.y, t);
			}
		}
		if (flowers[i].blooming) {
			double now = glfwGetTime();
			double elapsed = now - flowers[i].startTime;
			float t = elapsed / bloomDuration;
			if (elapsed >= bloomDuration) {
				flowers[i].blooming = false;
				flowers[i].decayed = false;
				flowers[i].currScale = 1.0;
			}
			else {
				flowers[i].currScale = glm::mix(0.0, 1.0, t);
				//std::cout << "current scale of flower: " << i << ": " << flowers[i].currScale << std::endl;
			}
		}
	}


	glUseProgram(shaderProgram);
	glUniform1f(glGetUniformLocation(shaderProgram, "percentage"), percent);
}

void HealthBar::SetTexture(GLuint texture) {
	healthTexture = texture;
}

void HealthBar::Draw() {
	glUseProgram(shaderProgram);
	glm::mat4 model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniform1i(glGetUniformLocation(shaderProgram, "isBranch"), 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, healthTexture);

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(VAO);
	if (isAlive) {
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	//display each flower
	glBindVertexArray(FlowerVAO);
	for (FlowerElement f : flowers) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(f.currPos.x, f.currPos.y, 0.0f));
		model = glm::scale(model, glm::vec3(f.currScale, f.currScale, 1.0f));
		model = glm::translate(model, glm::vec3(-flowerWidth / 2.0, -flowerWidth / 2.0, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
		glUniform1i(glGetUniformLocation(shaderProgram, "isBranch"), 0);
		float len = glm::abs(f.branchPos.y - f.endPos.y);
		float curr = glm::abs(f.branchPos.y - f.currPos.y);
		glUniform1f(glGetUniformLocation(shaderProgram, "decay"), curr / len);
		glBindTexture(GL_TEXTURE_2D, f.flowerTex);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void HealthBar::StartRegrow() {
	if (animating) {
		return;
	}

	//Set bool flag
	animStart = glfwGetTime();
	animating = true;

	for (int i = 0; i < flowers.size(); i++) {
		flowers[i].currScale = 0.0;
		flowers[i].decayed = true;
		flowers[i].blooming = false;
	}
}

void Magic::Init(std::vector<float> startPerc, float p, float r) {
	scHeight = WINDOWHEIGHT;
	scWidth = WINDOWWIDTH;
	percent = p;
	ratio = r;
	percX = startPerc[0];
	percY = startPerc[1];
	position = { percX * WINDOWWIDTH, percY * WINDOWHEIGHT };
	uiWidth = WINDOWWIDTH * percent;
	uiHeight = WINDOWHEIGHT * percent * 1.3 * ratio;
	centerX = position[0] + (uiWidth / 2.0f) + 15;
	centerY = position[1] + (uiHeight / 2.0f) - 15;

	manaWidth = WINDOWWIDTH * 0.1;

	shaderProgram = LoadShaders("shaders/ui.vert", "shaders/ui.frag");
	manaProgram = LoadShaders("shaders/magic.vert", "shaders/magic.frag");
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);

	float offsetX = uiWidth;
	float offsetY = uiHeight;

	quad = {
		//Position                                     //UV         //Color
		position[0], position[1],                      0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		position[0] + offsetX, position[1],            1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		position[0] + offsetX, position[1] + offsetY,  1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
		position[0], position[1] + offsetY,            0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
	};

	mana = {
		//Position                     //UV         //Color
		0.0f, 0.0f,                    0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		manaWidth, 0.0f,               1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		manaWidth, manaWidth,          1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
		0.0f, manaWidth,               0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
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
	glBufferData(GL_ARRAY_BUFFER, quad.size() * sizeof(float), quad.data(), GL_DYNAMIC_DRAW);
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
	glBufferData(GL_ARRAY_BUFFER, mana.size() * sizeof(float), mana.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); //position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); //tex coord
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float))); //color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(0);
}

void Magic::Update(const UIData& p) {
	//UPDATE ALL ELEMENTS OF THE UI FROM WINDOWWIDTH/WINDOWHEIGHT
	UpdateLayout();

	//loop through each struct and update according to the player
	float waterPerc = float(p.currWater) / float(p.maxWater);
	float firePerc = float(p.currFire) / float(p.maxFire);
	float earthPerc = float(p.currEarth) / float(p.maxEarth);
	float woodPerc = float(p.currWood) / float(p.maxWood);
	float metalPerc = float(p.currMetal) / float(p.maxMetal);
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
	float radius = manaRadius * WINDOWWIDTH;
	if (animating) {
		double elapsed = now - animStart;
		float t = elapsed / spinDuration;
		if (elapsed >= spinDuration) {
			animating = false;
			for (int i = 0; i < size; i++) {
				powers[i].currIdx = powers[i].targetIdx;
				float angle = baseAngles[powers[i].currIdx];
				float x = centerX + cos(angle) * radius;
				float y = centerY + sin(angle) * radius;
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
				float x = centerX + cos(nowAngle) * radius;
				float y = centerY + sin(nowAngle) * radius;
				powers[i].position = glm::vec2(x, y);
			}
		}
	}
}

void Magic::UpdateLayout() {
	if (scHeight == WINDOWHEIGHT && scWidth == WINDOWWIDTH) {
		return;
	}
	scHeight = WINDOWHEIGHT;
	scWidth = WINDOWWIDTH;
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);
	uiWidth = WINDOWWIDTH * percent;
	uiHeight = WINDOWHEIGHT * percent * ratio;

	position[0] = percX * WINDOWWIDTH;
	position[1] = percY * WINDOWHEIGHT;

	centerX = position[0] + (uiWidth / 2.0f) + 15;
	centerY = position[1] + (uiHeight / 2.0f) - 15;
	manaWidth = WINDOWWIDTH * 0.1;
	float offsetX = uiWidth;
	float offsetY = uiHeight;

	quad = {
		//Position                                     //UV         //Color
		position[0], position[1],                      0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		position[0] + offsetX, position[1],            1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		position[0] + offsetX, position[1] + offsetY,  1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
		position[0], position[1] + offsetY,            0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, backVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, quad.size() * sizeof(float), quad.data());

	mana = {
		//Position                     //UV         //Color
		0.0f, 0.0f,                    0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		manaWidth, 0.0f,               1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
		manaWidth, manaWidth,          1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
		0.0f, manaWidth,               0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, elemVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mana.size() * sizeof(float), mana.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Loop through all the powers and update the position accordinly
	const float total = powers.size();
	for (int i = 0; i < total; i++) {
		float radius = manaRadius * WINDOWWIDTH;
		float x = centerX + cos(baseAngles[powers[i].currIdx]) * radius;
		float y = centerY + sin(baseAngles[powers[i].currIdx]) * radius;
		powers[i].position = glm::vec2(x, y);
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
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//loop through each struct, send the uniform for mana % then draw
	float seconds = glfwGetTime();
	glUseProgram(manaProgram);
	glUniformMatrix4fv(glGetUniformLocation(manaProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniform1f(glGetUniformLocation(manaProgram, "time"), seconds);
	glBindVertexArray(elemVAO);
	for (const auto& p : powers) {
		//std::cout << p.currMana << std::endl;
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

//0 1 2 3 4
void Magic::SetRotate(int anim) {
	if (animating) {
		return;
	}

	for (auto& p : powers) {
		int size = baseAngles.size();
		if (anim == 3) {
			int offset = size - powers.at(0).currIdx;
			p.targetIdx = (p.targetIdx + offset) % size;
		}
		else if (anim == 4) {
			int offset = size - powers.at(1).currIdx;
			p.targetIdx = (p.targetIdx + offset) % size;
		}
		else if (anim == 5) {
			int offset = size - powers.at(2).currIdx;
			p.targetIdx = (p.targetIdx + offset) % size;
		}
		else if (anim == 6) {
			int offset = size - powers.at(3).currIdx;
			p.targetIdx = (p.targetIdx + offset) % size;
		}
		else if (anim == 7) {
			int offset = size - powers.at(4).currIdx;
			p.targetIdx = (p.targetIdx + offset) % size;
		}
	}

	animStart = glfwGetTime();
	animating = true;
}

void Vignette::Init(std::vector<float> startPos, float percent, float ratio) {
	shaderProgram = LoadShaders("shaders/vignette.vert", "shaders/vignette.frag");
	projection = glm::ortho(0.0f, float(WINDOWWIDTH), 0.0f, float(WINDOWHEIGHT), -1.0f, 1.0f);

	//goldTexture = textures["gold"];
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	uiData = {
		//Position												   //UV         //Color
		startPos[0], startPos[1], 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + WINDOWWIDTH, startPos[1], 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		startPos[0] + WINDOWWIDTH, startPos[1] + WINDOWHEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		startPos[0], startPos[1] + WINDOWHEIGHT, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
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

void Vignette::Update(const UIData& p) {
	lastHealth = p.currHP;
	if (lastHealth <= 0) {
		isAlive = false;
		isLow = false;
	}
	else if (lastHealth <= 20) {
		isLow = true;
		isAlive = true;
	}
	else {
		isLow = false;
		isAlive = true;
	}
	hasFlag = client->GameState.player_stats[client->playerId].hasFlag;
}

void Vignette::Draw() {
	glUseProgram(shaderProgram);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO);

	glm::mat4 model = glm::mat4(1.0f);
	float seconds = glfwGetTime();
	glUniform1f(glGetUniformLocation(shaderProgram, "time"), seconds);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniform1i(glGetUniformLocation(shaderProgram, "isAlive"), isAlive);
	glUniform1i(glGetUniformLocation(shaderProgram, "isLow"), isLow);
	glUniform1i(glGetUniformLocation(shaderProgram, "hasFlag"), hasFlag);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, goldTexture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

void Vignette::SetTexture(GLuint tex) {
	texture = tex;
}
