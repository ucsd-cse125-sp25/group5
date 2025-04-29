#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "shared/NetworkData.h"

class ClientGame
{
public:
	ClientGame(void);
	~ClientGame(void);

	ClientNetwork* network;

	void sendActionPackets(PlayerIntentPacket intent);

    char network_data[MAX_PACKET_SIZE];

    void update(PlayerIntentPacket intent);

	GameStatePacket GameState;
	int playerId = -1;
	glm::mat4 playerModel = glm::mat4(1.0f);
};

