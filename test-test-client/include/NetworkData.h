#pragma once
#include <string.h> 
#include "core.h"
#include <glm/gtc/type_ptr.hpp>


#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

    INIT_CONNECTION = 0,

    ACTION_EVENT = 1,

};

struct Packet {

    unsigned int packet_type;
    char buf[128];

    void serialize(char * data) {
        memcpy(data, this, sizeof(Packet));
    }

    void deserialize(char * data) {
        memcpy(this, data, sizeof(Packet));
    }
};

struct PlayerIntentPacket {
    bool moveLeftIntent = false;
    bool moveRightIntent = false;
    bool moveUpIntent = false;
    bool moveDownIntent = false;

	void serialize(char* data) {
		memcpy(data, this, sizeof(PlayerIntentPacket));
	}

	void deserialize(char* data) {
		memcpy(this, data, sizeof(PlayerIntentPacket));
	}
};

struct GameStatePacket {
    float cubeModel[16];  // flat float array

    void serialize(char* data) const {
        memcpy(data, cubeModel, sizeof(cubeModel));
    }

    void deserialize(const char* data) {
        memcpy(cubeModel, data, sizeof(cubeModel));
    }

    glm::mat4 getModelMatrix() const {
        glm::mat4 mat;
        memcpy(glm::value_ptr(mat), cubeModel, sizeof(cubeModel));
        return mat;
    }

    void setModelMatrix(const glm::mat4& mat) {
        memcpy(cubeModel, glm::value_ptr(mat), sizeof(cubeModel));
    }

    void print() const {
        for (int i = 0; i < 16; ++i) {
            printf("%f ", cubeModel[i]);
        }
        printf("\n");
    }
};