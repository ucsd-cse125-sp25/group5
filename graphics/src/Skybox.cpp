#include "Skybox.h"
#include "stb_image.h"

extern double currTime;
extern double startTime;

extern float waterLevel;
extern float fogConstant;
extern float fogConstantW;
extern glm::vec3 fogColor;
extern glm::vec3 fogColorW;

glm::vec3 getViewDir(float azimuth, float incline) {
    float x = cos(incline) * sin(azimuth);
    float y = sin(incline);
    float z = cos(incline) * cos(azimuth);
    return glm::normalize(glm::vec3(x, y, z));
}

// Will populate the VAO, VBOs, and textures
void Skybox::initSkybox() {
    float skyVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), &skyVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    faces = {
        PROJECT_SOURCE_DIR + std::string("/assets/right.png"),
        PROJECT_SOURCE_DIR + std::string("/assets/left_quartermoon.png"),
        PROJECT_SOURCE_DIR + std::string("/assets/top.png"),
        PROJECT_SOURCE_DIR + std::string("/assets/bottom.png"),
        PROJECT_SOURCE_DIR + std::string("/assets/front.png"),
        PROJECT_SOURCE_DIR + std::string("/assets/back.png")
    };

    ct1 = loadCubemap(faces);

    faces[1] = PROJECT_SOURCE_DIR + std::string("/assets/left_gibbousmoon.png");
    ct2 = loadCubemap(faces);

    faces[1] = PROJECT_SOURCE_DIR + std::string("/assets/left_fullmoon.png");
    ct3 = loadCubemap(faces);

    cubemapTexture = ct1;

    shaderProgram = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");
}


//Loads the cubemap textures from file paths
GLuint Skybox::loadCubemap(const std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++) {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            std::cout << "Found the data for the faces!!!!" << std::endl;
            GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Skybox::draw(Camera* cam, float* model) {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL); //change depth function so depth test passes when values are equal to depth buffer's content
    glUseProgram(shaderProgram);
    glm::mat4 projection = cam->GetProjMtx();
    glm::mat4 view = glm::mat4(glm::mat3(cam->GetViewMtx())); //remove translation from the matrix
    glm::mat4 viewProj = projection * view;
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewProj"), 1, GL_FALSE, &viewProj[0][0]);
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), (currTime - startTime) / 1000.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "waterLevel"), waterLevel);
    glUniform1f(glGetUniformLocation(shaderProgram, "fogConstant"), 0.05f);
    glUniform1f(glGetUniformLocation(shaderProgram, "fogConstantW"), 0.2f);
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, &fogColor[0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColorW"), 1, &fogColorW[0]);
    glm::vec3 camPos = cam->GetPosition();
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &camPos[0]);
    glUniform4fv(glGetUniformLocation(shaderProgram, "waterModel"), 1, model);
   
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void Skybox::updatePhase(int phase) {
    if (phase == 1) {
        cubemapTexture = ct2;
    }
    else if (phase == 2) {
        cubemapTexture = ct3;
    }
}