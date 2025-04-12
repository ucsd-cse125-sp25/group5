#include "StdAfx.h"
#include "ClientGame.h"


ClientGame::ClientGame(void)
{

    network = new ClientNetwork();

    // send init packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = INIT_CONNECTION;

    packet.serialize(packet_data);

    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);

    GameState = 0;
}


ClientGame::~ClientGame(void)
{
}

void ClientGame::sendActionPackets(int packet_type)
{
    // send action packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = packet_type;

    packet.serialize(packet_data);

    NetworkServices::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::update(bool leftDown)
{
    Packet packet;
    int data_length = network->receivePackets(network_data);

    if (data_length <= 0) 
    {
        
    }
    else {
        int i = 0;
        while (i < (unsigned int)data_length)
        {
            packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);

            switch (packet.packet_type) {

            case ACTION_EVENT:

                printf("client received action event packet from server\n");

                GameState = 1;

                break;

            case 2:

                printf("client received 2 event packet from server\n");

                GameState = 2;

            default:

                printf("error in packet types\n");

                break;
            }
        }
    }

    if (leftDown) {
        sendActionPackets(2);
    }   
    else {
        sendActionPackets(1);
    }
}
