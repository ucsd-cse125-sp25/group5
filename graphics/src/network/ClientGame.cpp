#include "network/StdAfx.h"
#include "network/ClientGame.h"
#include "shared/NetworkData.h"
#include <chrono>


// Declare variables for time tracking
std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
std::chrono::time_point<std::chrono::high_resolution_clock> lastPacketSentTime;

ClientGame::ClientGame(void)
{
    network = new ClientNetwork();

	//initialize game state
	GameState = GameStatePacket();
	//GameState.setModelMatrix(glm::mat4(1.0f));
	playerModel = glm::mat4(1.0f);

	//PlayerIntent = PlayerIntentPacket();
    currentTime = std::chrono::high_resolution_clock::now();
    lastPacketSentTime = std::chrono::high_resolution_clock::now();

}

ClientGame::~ClientGame(void)
{
}

void ClientGame::sendActionPackets(PlayerIntentPacket intent)
{
    // send action packet
    const unsigned int packet_size = sizeof(PlayerIntentPacket);
    char packet_data[packet_size];

    intent.serialize(packet_data);

	//printf("client sending action packet to server with the following data: %d %d %d %d\n", intent.moveLeftIntent, intent.moveRightIntent, intent.moveDownIntent, intent.moveUpIntent);    
    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

//receive packets
void ClientGame::update(PlayerIntentPacket intent)
{
    int data_length = network->receivePackets(network_data);
	//printf("data length: %d\n", data_length);
    if (data_length <= 0 ) 
    {
        //sendActionPackets(intent);
    }
    else {
        int i = 0;
        while (i < (unsigned int)data_length)
        {
            if (network_data[i] == JOIN_RESPONSE) {
                JoinResponsePacket packet;
                packet.deserialize(&(network_data[i]));
                // Set playerId on JoinResponse from server
                playerId = packet.entity_id;
                i += sizeof(JoinResponsePacket);

                //printf("Received playerId %d from server\n", packet.entity_id);
                continue;
            }

            GameStatePacket packet;
            packet.deserialize(&(network_data[i]));
            memcpy(&GameState, &packet, sizeof(GameStatePacket));
			i += sizeof(GameStatePacket);

            // Update playerModel on receiving GameState
            for (int j = 0; j < GameState.num_players; j++) {
                if (GameState.players[j].id == playerId) {
                    playerModel = GameState.players[j].model;
                    break;
                }
            }
			//printf("Received num_entities: %d\n", GameState.num_entities);
			//printf("First entity is of type %d\n", GameState.entities[0].type);
        }
    }

	// throttle the packets
	currentTime = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration<float>(currentTime - lastPacketSentTime).count() > 0.016f) {
		lastPacketSentTime = currentTime;
		sendActionPackets(intent);
	}
}
