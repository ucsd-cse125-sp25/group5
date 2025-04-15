#include "StdAfx.h"
#include "ServerGame.h"

unsigned int ServerGame::client_id; 
float posX{};
float posY{};
float posZ{};

ServerGame::ServerGame(void)
{
    // id's to assign clients for our table
    client_id = 0;

    // set up the server network to listen 
    network = new ServerNetwork();
}

ServerGame::~ServerGame(void)
{
}

void ServerGame::update()
{
    // get new clients
   if(network->acceptNewClient(client_id))
   {
        printf("client %d has been connected to the server\n",client_id);

        client_id++;
   }

   receiveFromClients();
}

void ServerGame::receiveFromClients()
{

    Packet packet;

    // go through all clients
    std::map<unsigned int, SOCKET>::iterator iter;

    for(iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
    {
        int data_length = network->receiveData(iter->first, network_data);

        if (data_length <= 0) 
        {
            //no data recieved
            continue;
        }

        int i = 0;
        bool move = false;

        while (i < (unsigned int)data_length) 
        {
            packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);

            switch (packet.packet_type) {

                case INIT_CONNECTION:

                    printf("server received init packet from client\n");

                    sendActionPackets(ACTION_EVENT);

                    break;

                case ACTION_EVENT:

                    printf("server received action event packet from client\n");

                    sendActionPackets(ACTION_EVENT);

                    break;

                case FORWARD:
                    printf("server received FORWARD packet from client\n");
                    posZ -= 1.0f;
                    move = true;
                    break;
                case BACKWARD:
                    printf("server received BACKWARD packet from client\n");
                    posZ += 1.0f;
                    move = true;
                    break;
                case LEFT:
                    printf("server received LEFT packet from client\n");
                    posX -= 1.0f;
                    move = true;
                    break;
                case RIGHT:
                    printf("server received RIGHT packet from client\n");
                    posX += 1.0f;
                    move = true;
                    break;
                default:

                    printf("error in packet types\n");

                    break;
            }

            if (move) {
                PositionPacket packet;
                packet.packet_type = POSITION;
                packet.x = posX;
                packet.y = posY;
                packet.z = posZ;

                printf("x=%f,y=%f,z=%f\n", packet.x, packet.y, packet.z);

                const unsigned int packet_size = sizeof(PositionPacket);
                char packet_data[packet_size];
                packet.serialize(packet_data);
                network->sendToAll(packet_data, packet_size);
            }
            
        }
    }
}


void ServerGame::sendActionPackets(int packet_type)
{
    // send action packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = packet_type;

    packet.serialize(packet_data);

    network->sendToAll(packet_data,packet_size);
}