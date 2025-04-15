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

void ClientGame::update()
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
            UINT8 type = network_data[i];
            printf("type=%d\n", type);

            if (type == POSITION) {
                printf("client received POSITION packet from server\n");

                PositionPacket packet;
                packet.deserialize(&(network_data[i]));
                printf("x=%f,y=%f,z=%f\n", packet.x, packet.y, packet.z);
                this->nX = packet.x;
                this->nY = packet.y;
                this->nZ = packet.z;
                i += sizeof(PositionPacket);
                continue;
            }

            packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);

            switch (packet.packet_type) {

            case ACTION_EVENT:

                printf("client received action event packet from server\n");

                //GameState = 1;

                break;

            case 2:

                printf("client received 2 event packet from server\n");

                //GameState = 2;

            default:

                printf("error in packet types\n");

                break;
            }
        }
    }
}
