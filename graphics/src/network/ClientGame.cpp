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

	//printf("client sending action packet to server with the following data: %d %d %d %d\n", intent.mofffffeftIntent, intent.moveRightIntent, intent.moveDownIntent, intent.moveUpIntent);    
    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}
char rem[MAX_PACKET_SIZE];
int remSize = 0;
PacketTypes type = NONE;

char network_data[MAX_PACKET_SIZE];

void ClientGame::handleGameStatePacket(char* buf) {
    GameState.deserialize(buf);

    // Update playerModel on receiving GameState
    for (int j = 0; j < GameState.num_players; j++) {
        if (GameState.players[j].id == playerId) {
            playerModel = GameState.players[j].model;
            break;
        }
    }
}

//receive packets
void ClientGame::update(PlayerIntentPacket intent)
{
    int bytes_received = network->receivePackets(network_data);

    if (bytes_received >= 0) {
        int i = 0;

        // There is partial packet stored in rem
        if (type != NONE) {
            int bytes_required = sizeof(GameStatePacket) - remSize;

            // buffer STILL has less bytes than what's needed for processing a full GameState Packet
            if (bytes_received < bytes_required) {
                memcpy(&(rem[remSize]), network_data, bytes_received);
                remSize += bytes_received;
                type = GAME_STATE;
                i += bytes_received;
            }
            else {
                memcpy(&(rem[remSize]), network_data, bytes_required);
                handleGameStatePacket(rem);
                remSize = 0;
                type = NONE;
                i += bytes_required;
            }
        }

        while (i < bytes_received) {
            if (network_data[i] == JOIN_RESPONSE) {
                JoinResponsePacket packet;
                packet.deserialize(&(network_data[i]));
                // Set playerId on JoinResponse from server
                playerId = packet.entity_id;
                i += sizeof(JoinResponsePacket);

                //printf("Received playerId %d from server\n", packet.entity_id);
                continue;
            }

            // GameState Packet
            int bytes_required = sizeof(GameStatePacket);
            int bytes_remaining = bytes_received - i;

            // buffer has less bytes than what's needed for processing a full GameState Packet
            if (bytes_remaining < bytes_required) {
                memcpy(rem, &(network_data[i]), bytes_remaining);
                remSize = bytes_remaining;
                type = GAME_STATE;
                break;
            }

            handleGameStatePacket(&(network_data[i]));
            i += sizeof(GameStatePacket);
        }
    }

    // throttle the packets
    currentTime = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration<float>(currentTime - lastPacketSentTime).count() > 0.016f) {
        lastPacketSentTime = currentTime;
        sendActionPackets(intent);
    }
}