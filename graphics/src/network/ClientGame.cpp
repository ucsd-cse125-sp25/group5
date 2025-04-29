#include "network/StdAfx.h"
#include "network/ClientGame.h"
#include "network/NetworkData.h"
#include <chrono>


// Declare variables for time tracking
std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
std::chrono::time_point<std::chrono::high_resolution_clock> lastPacketSentTime;



ClientGame::ClientGame(void)
{

    network = new ClientNetwork();

    // don't send init packet
    /*const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = INIT_CONNECTION;

    packet.serialize(packet_data);

    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);*/

	//initialize game state
	GameState = GameStatePacket();
	//GameState.setModelMatrix(glm::mat4(1.0f));
	GameState.cubeModel = glm::mat4(1.0f);


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
	GameStatePacket packet = GameStatePacket();
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

            packet.deserialize(&(network_data[i]));
			i += sizeof(GameStatePacket); // read the packet type first


			//printf("client received game state packet from server with the following mat4:");
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < 4; k++) {
					//printf("%f ", GameState.getModelMatrix()[i][j]);
					//printf("%f ", packet.cubeModel[k][j]);
                }
				//printf("\n");
            }

			//packet.cubeModel = glm::mat4(1.0f);
			memcpy(&GameState, &packet, sizeof(GameStatePacket));
            //printf("\n");
        }
    }
       

	//throttle the packets
	currentTime = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration<float>(currentTime - lastPacketSentTime).count() > 0.016f) {
		lastPacketSentTime = currentTime;
		sendActionPackets(intent);
	}

    //printf("Reached here \n");


    /*if (leftDown) {
        sendActionPackets(2);
    }   
    else {
        sendActionPackets(1);
    }*/
    //sendActionPackets(intent);
}
