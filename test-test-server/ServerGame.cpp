#include "StdAfx.h"
#include "ServerGame.h"
#include <chrono>
#include <random>

unsigned int ServerGame::client_id; 

//time now

#include <chrono> // Ensure this is included for time-related functionality

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

    //the current game state TO SEND (not necessarily full game state)
	GameState = GameStatePacket();

	//the current player intent received
	PlayerIntent = PlayerIntentPacket();

    //initialize the physics system
    physicsSystem = PhysicsSystem();

    //boilerplate
	GameObject* cube = physicsSystem.makeGameObject();


    //create a random number of cubes to put in the world
    /*random no of cubes*/
	int numCubes = rand() % 30 + 1; // Random number between 1 and 10
        
    for (int i = 0; i < numCubes; i++) {
		GameObject* cube = physicsSystem.makeGameObject();
		cube->transform.position = glm::vec3(rand() % 10, rand() % 10, rand() % 10);
		cube->transform.rotation = glm::vec3(0.0f);
		cube->transform.scale = glm::vec3(1.0f);
		cube->physics->velocity = glm::vec3(0.0f);
		cube->physics->acceleration = glm::vec3(0.0f);
		cube->physics->drag = 0.1f;
		cube->physics->maxSpeed = 5.0f;
		physicsSystem.addDynamicObject(cube);
    }


	//GameState.setModelMatrix(glm::mat4(1.0f)); // Initialize the cube model matrix
	GameState.cubeModel = glm::mat4(1.0f); // Initialize the cube model matrix
    physicsSystem.players[0] = cube;
    
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

void ServerGame::writeToGameState() {

    // Update the player (first object) in the GameState
    glm::vec3& playerPositionReference = physicsSystem.players[0]->transform.position;
    glm::vec3& playerRotationReference = physicsSystem.players[0]->transform.rotation;
    glm::mat4 playerModel = physicsSystem.toMatrix(playerPositionReference, playerRotationReference);
    GameState.cubeModel = playerModel;

    // Update all other objects in the GameState
    int numEntities = physicsSystem.dynamicObjects.size() + physicsSystem.staticObjects.size();
    GameState.num_objects = numEntities;

    //send all the dynamic objects
    for (int i = 0; i < physicsSystem.dynamicObjects.size(); i++) {
        GameObject* obj = physicsSystem.dynamicObjects[i];
        glm::vec3& position = obj->transform.position;
        glm::vec3& rotation = obj->transform.rotation;
        glm::mat4 modelMatrix = physicsSystem.toMatrix(position, rotation);

        // Assuming GameState has a way to store multiple objects' model matrices
        GameState.objects[i] = modelMatrix; // Replace with actual storage logic
    }

    //send all the static objects
    for (int i = 0; i < physicsSystem.staticObjects.size(); i++) {
        GameObject* obj = physicsSystem.staticObjects[i];
        glm::vec3& position = obj->transform.position;
        glm::vec3& rotation = obj->transform.rotation;
        glm::mat4 modelMatrix = physicsSystem.toMatrix(position, rotation);

        // Assuming GameState has a way to store multiple objects' model matrices
        GameState.objects[i + physicsSystem.dynamicObjects.size()] = modelMatrix; // Replace with actual storage logic
    }
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

            //copy the network packet data into player intent
            PlayerIntent.deserialize(&(network_data[i]));

            //increment in case we have more 
            i += sizeof(PlayerIntentPacket);

            //apply the input to our game world, assume player 0 for now 
			physicsSystem.applyInput(PlayerIntent, 0); 

			//put new information into the game state
			writeToGameState();

			//updated game state   
            currentTime = std::chrono::high_resolution_clock::now();

            if (std::chrono::duration<float>(currentTime - lastPacketSentTime).count() > 0.033f) // some fixed constant
            {
                lastPacketSentTime = currentTime;
                sendActionPackets();
            }
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
    printf("server sending game state packet from server with the following mat4:");
    for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++) {
            //printf("%f ", GameState.getModelMatrix()[i][j]);
            printf("%f ", GameState.cubeModel[k][j]);
        }
    }
    printf("\n");

    GameState.serialize(packet_data);

    network->sendToAll(packet_data, packet_size);
}


