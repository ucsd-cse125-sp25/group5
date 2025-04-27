#pragma once
#include <string.h> 
#include "core.h"
#include "ObjectData.h"

#define MAX_PACKET_SIZE 1000000
#define MAX_ENTITIES 128
#define MAX_PLAYERS 4

enum PacketTypes {

    PLAYER_INTENT = 0,
    JOIN_RESPONSE = 1,
    GAME_STATE = 2

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
    bool moveForwardIntent = false;
    bool moveBackIntent = false;
    float azimuthIntent = 0.0f;
    float inclineIntent = 0.0f;

    void serialize(char* data) {
        memcpy(data, this, sizeof(PlayerIntentPacket));
    }

    void deserialize(char* data) {
        memcpy(this, data, sizeof(PlayerIntentPacket));
    }
};

struct JoinResponsePacket {
    unsigned int packet_type;

    unsigned int entity_id;

    void serialize(char* data) {
        memcpy(data, this, sizeof(JoinResponsePacket));
    }

    void deserialize(char* data) {
        memcpy(this, data, sizeof(JoinResponsePacket));
    }
};

struct GameStatePacket {

    unsigned int packet_type;

    unsigned int num_players;
    struct Entity players[MAX_ENTITIES];

    unsigned int num_entities;
    struct Entity entities[MAX_PLAYERS];

    void serialize(char* data) {
        memcpy(data, this, sizeof(GameStatePacket));
    }

    void deserialize(char* data) {
        memcpy(this, data, sizeof(GameStatePacket));
    }
};