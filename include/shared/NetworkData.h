#pragma once
#include <string.h> 
#include "core.h"
#include "ObjectData.h"

#define MAX_PACKET_SIZE 1000000
#define MAX_ENTITIES 1024
#define MAX_PLAYERS 4

const static int KILLFEED_LENGTH = 3;

enum PacketTypes {

	PLAYER_INTENT = 0,
	JOIN_RESPONSE = 1,
	GAME_STATE = 2,
	NONE = 3

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

	bool scrollIntentTriggered = false;

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

	PowerType changeToPower = METAL;

	void serialize(char* data) {
		memcpy(data, this, sizeof(PlayerIntentPacket));
	}

	void deserialize(char* data) {
		memcpy(this, data, sizeof(PlayerIntentPacket));
	}
};

/**
 * @struct JoinResponsePacket
 * @brief Represents a packet sent in response to a join request in the network.
 *
 * This structure contains information about the type of the packet and the
 * entity ID assigned to the client. It also provides methods for serializing
 * and deserializing the packet data for network transmission.
 *
 * @var JoinResponsePacket::packet_type
 * The type of the packet, typically used to identify the purpose of the packet.
 *
 * @var JoinResponsePacket::entity_id
 * The unique identifier assigned to the entity in the network.
 *
 * @fn JoinResponsePacket::serialize(char* data)
 * @brief Serializes the packet data into a raw byte array.
 * @param data A pointer to the destination buffer where the serialized data will be stored.
 *
 * @fn JoinResponsePacket::deserialize(char* data)
 * @brief Deserializes the packet data from a raw byte array.
 * @param data A pointer to the source buffer containing the serialized data.
 */
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

	GamePhase phase;
	int time;

	//flag for moon
	MoonPhase moonPhase;
	unsigned int num_players;
	struct Entity players[MAX_PLAYERS];

  //water level
  float waterLevel = -2.0f; // Initial water level
  
	unsigned int num_entities;
	struct Entity entities[MAX_ENTITIES];

	struct KillfeedItem killfeed[KILLFEED_LENGTH];

	int lockedWinnerId = -1; // -1 means no one has won yet

	void serialize(char* data) {
		memcpy(data, this, sizeof(GameStatePacket));
	}

	void deserialize(char* data) {
		memcpy(this, data, sizeof(GameStatePacket));
	}
};