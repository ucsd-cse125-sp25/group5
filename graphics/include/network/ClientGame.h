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
	uint16_t id;
};

