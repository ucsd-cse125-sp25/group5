#pragma once
#include <string.h> 
#include "core.h"
#include "ObjectData.h"

#define MAX_PACKET_SIZE 1000000
#define MAX_ENTITIES 1024
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

    //movement
    bool moveLeftIntent = false;
    bool moveRightIntent = false;
    bool moveUpIntent = false;
    bool moveDownIntent = false;
    bool moveForwardIntent = false;
    bool moveBackIntent = false;
    float azimuthIntent = 0.0f;
    float inclineIntent = 0.0f;

    //attack triggers
	bool rightClickIntent = false;
	bool leftClickIntent = false;
    bool scrollUpIntent = false;
	bool scrollDownIntent = false;

    //powers
	bool hit1Intent = false;
	bool hit2Intent = false;
	bool hit3Intent = false;
	bool hit4Intent = false;
	bool hit5Intent = false;

	bool hitEIntent = false;
	bool hitRIntent = false;
	bool hitTIntent = false;
	bool hitYIntent = false;
	bool hitUIntent = false;

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

	struct PlayerStats player_stats[MAX_PLAYERS];

    unsigned int num_players;
    struct Entity players[MAX_PLAYERS];

    unsigned int num_entities;
    struct Entity entities[MAX_ENTITIES];

    void serialize(char* data) {
        memcpy(data, this, sizeof(GameStatePacket));
    }

    void deserialize(char* data) {
        memcpy(this, data, sizeof(GameStatePacket));
    }
};