#include "StdAfx.h"
#include "ServerGame.h"
#include <chrono>

unsigned int ServerGame::client_id; 

//time now

#include <chrono> // Ensure this is included for time-related functionality
#include <glm/gtx/euler_angles.hpp>

// Declare variables for time tracking
std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
std::chrono::time_point<std::chrono::high_resolution_clock> lastPacketSentTime;


ServerGame::ServerGame(void)
{
    // id's to assign clients for our table
    client_id = 0;

    // set up the server network to listen 
    network = new ServerNetwork(); 
    glm::vec3(1, 1, 1);
	GameState = GameStatePacket();
	PlayerIntent = PlayerIntentPacket();

    /*Initialize the GameState*/

	//GameState.setModelMatrix(glm::mat4(1.0f)); // Initialize the cube model matrix
	GameState.cubeModel = glm::mat4(1.0f); // Initialize the cube model matrix
    
	currentTime = std::chrono::high_resolution_clock::now();
	lastPacketSentTime = std::chrono::high_resolution_clock::now();
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

   //currentTime = std::chrono::high_resolution_clock::now();

   //if (std::chrono::duration<float>(currentTime - lastPacketSentTime).count() > 0.33f) // some fixed constant
   //{
   //    lastPacketSentTime = currentTime;
   //    sendActionPackets();
   //}
}

void ServerGame::receiveFromClients()
{

    //Packet packet;

    // go through all clients
    std::map<unsigned int, SOCKET>::iterator iter;

    for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++)
    {
        int data_length = network->receiveData(iter->first, network_data);

        if (data_length <= 0)
        {
            //no data recieved
            continue;
        }

        int i = 0;
        while (i < (unsigned int)data_length)
        {
            /*packet.deserialize(&(network_data[i]));
            i += sizeof(Packet);*/

            PlayerIntent.deserialize(&(network_data[i]));

			/*printf("server received player intent packet from client with the following data:\n");
            printf("moveLeftIntent: %s\n", PlayerIntent.moveLeftIntent ? "true" : "false");
            printf("moveRightIntent: %s\n", PlayerIntent.moveRightIntent ? "true" : "false");
            printf("moveUpIntent: %s\n", PlayerIntent.moveUpIntent ? "true" : "false");
            printf("moveDownIntent: %s\n", PlayerIntent.moveDownIntent ? "true" : "false");*/

            i += sizeof(PlayerIntentPacket);
            //print before
			//printf("cubeModel before: \n");
			//printf("x: %f, y: %f, z: %f\n", GameState.cubeModel[3][0], GameState.cubeModel[3][1], GameState.cubeModel[3][2]);
            //Pulling the translation from the cube and resetting the 

            //Reset cube model
            glm::vec3 delta = glm::vec3(0.016f);
            float azimuth = glm::radians(-PlayerIntent.azimuthIntent);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), azimuth, up);
            glm::vec3 forward = glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
            glm::vec3 translation = glm::vec3(GameState.cubeModel[3]);
            glm::vec3 right = glm::normalize(glm::cross(up, forward));

            //GameState.cubeModel = glm::rotate(GameState.cubeModel, azimuth, glm::vec3(0.0f, 1.0f, 0.0f));

            //process
            if (PlayerIntent.moveLeftIntent)
            {
                translation +=  (- right) * delta;
				//GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(-0.1f, 0.0f, 0.0f));
                //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(-0.1f, 0.0f, 0.0f));
            }
            if (PlayerIntent.moveRightIntent) {
                translation += right * delta;
                //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.1f, 0.0f, 0.0f));
				//GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(0.1f, 0.0f, 0.0f)));
            }
            if (PlayerIntent.moveUpIntent) {
                translation += up * delta;
                //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, 0.1f, 0.0f));
				//GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(0.0f, 0.1f, 0.0f)));

            }
            if (PlayerIntent.moveDownIntent) {
                translation += (-up) * delta;
                //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, -0.1f, 0.0f));
				//GameState.setModelMatrix(glm::translate(GameState.getModelMatrix(), glm::vec3(0.0f, -0.1f, 0.0f)));
            }
            if (PlayerIntent.moveForwardIntent) {
                translation += (-forward) * delta;
                //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, 0.0f, -0.1f));
            }
            if (PlayerIntent.moveBackIntent) {
                translation += forward * delta;
                //GameState.cubeModel = glm::translate(GameState.cubeModel, glm::vec3(0.0f, 0.0f, 0.1f));
            }

            //glm::mat4 id = glm::mat4(1.0f);
            //GameState.cubeModel = glm::translate(GameState.cubeModel, translation);
            GameState.cubeModel = rotation;
            GameState.cubeModel[3] = glm::vec4(translation, 1.0f);

         
            //GameState.cubeModel *= glm::eulerAngleY(glm::radians(-PlayerIntent.azimuthIntent)) * glm::eulerAngleX(glm::radians(-PlayerIntent.inclineIntent));
        /*    GameState.cubeModel = glm::rotate(GameState.cubeModel, PlayerIntent.azimuthIntent, glm::vec3(0.0f, 1.0f, 0.0f));
            GameState.cubeModel = glm::rotate(GameState.cubeModel, PlayerIntent.inclineIntent, glm::vec3(1.0f, 0.0f, 0.0f));*/

            sendActionPackets();
			//print after
			//printf("cubeModel after: \n");
			//printf("x: %f, y: %f, z: %f\n", GameState.cubeModel[3][0], GameState.cubeModel[3][1], GameState.cubeModel[3][2]);

            /*switch (packet.packet_type) {

                case INIT_CONNECTION:

                    printf("server received init packet from client\n");

                    sendActionPackets(ACTION_EVENT);

                    break;

                case ACTION_EVENT:

                    printf("server received action event packet from client\n");

                    sendActionPackets(ACTION_EVENT);

                    break;

                case 2:

                    printf("server received defend event from client\n");

                    sendActionPackets(2);

                    break;

                default:

                    printf("error in packet types\n");

                    break;
            }
        }*/
        }
    }
}

void ServerGame::sendActionPackets()
{
    // send action packet
    const unsigned int packet_size = sizeof(GameStatePacket);
    char packet_data[packet_size];

    //GameStatePacket packet;
    //packet.cubeModel = GameState.cubeModel;
    
    //printf("about to send x: %f, y: %f, z: %f\n", GameState.cubeModel[3][0], GameState.cubeModel[3][1], GameState.cubeModel[3][2]);
    //printf("cube model is: ");
    //for (int j = 0; j < 4; j++) {
    //    for (int k = 0; k < 4; k++) {
    //        //printf("%f ", GameState.getModelMatrix()[i][j]);
    //        printf("%f ", GameState.cubeModel[k][j]);
    //    }
    //    printf("\n");
    //}
    GameState.serialize(packet_data);

    network->sendToAll(packet_data, packet_size);
}


