#pragma once
#include <string.h> 


#define MAX_PACKET_SIZE 1000000
#define MAX_ENTITIES 128

enum PacketTypes {

    INIT_CONNECTION = 0,

    ACTION_EVENT = 1,

};

struct Packet {

    unsigned int packet_type;
    char buf[128];

    void serialize(char* data) {
        memcpy(data, this, sizeof(Packet));
    }

    void deserialize(char* data) {
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

    unsigned int packet_type;
    glm::mat4 cubeModel;

    unsigned int num_objects;
    glm::mat4 objects[MAX_ENTITIES];

    void serialize(char* data) {
        memcpy(data, this, sizeof(GameStatePacket));
    }

    void deserialize(char* data) {
        memcpy(this, data, sizeof(GameStatePacket));
    }
};