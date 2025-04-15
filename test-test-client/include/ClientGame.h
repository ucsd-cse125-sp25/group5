#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"

class ClientGame
{
public:
	ClientGame(void);
	~ClientGame(void);

	ClientNetwork* network;

	void sendActionPackets(int packet_type);

    char network_data[MAX_PACKET_SIZE];

    void update();

	float nX{};
	float nY{};
	float nZ{};
};

