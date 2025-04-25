#include "StdAfx.h"
#include "ServerGame.h"
#include <chrono>
#include <thread>
#include <random>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

unsigned int ServerGame::client_id;

std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

ServerGame::ServerGame(void)
{
    // id's to assign clients for our table
    client_id = 0;

    // set up the server network to listen 
    network = new ServerNetwork();

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
		cube->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
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
}

ServerGame::~ServerGame(void)
{
}

void ServerGame::update()
{
    startTime = std::chrono::high_resolution_clock::now();

    // get new clients
   if(network->acceptNewClient(client_id))
   {
        printf("client %d has been connected to the server\n",client_id);

        client_id++;
   }

   bool sendUpdate = receiveFromClients();

   if (sendUpdate) {
       //put new information into the game state
       writeToGameState();
       sendActionPackets();
   }

   endTime = std::chrono::high_resolution_clock::now();
   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

   if (duration < 50) // If did not spend the whole tick (50ms)
   {
       std::this_thread::sleep_for(std::chrono::milliseconds(duration));
   }
}




void ServerGame::writeToGameState() {

    // Update the player (first object) in the GameState
    glm::vec3& playerPositionReference = physicsSystem.players[0]->transform.position;
	//glm::vec3& playerPositionReference = glm::vec3(0.0f);
    glm::quat& playerRotationReference = physicsSystem.players[0]->transform.rotation;
    /*glm::mat4 playerModel = physicsSystem.toMatrix(playerPositionReference, playerRotationReference);*/
	//glm::mat4 playerModel = glm::toMat4(glm::quat(playerRotationReference));
	//playerModel[3] = glm::vec4(playerPositionReference, 1.0f); // Set the translation part of the matrix
 //   GameState.cubeModel = playerModel;

    glm::mat4 rotationMatrix = glm::toMat4(glm::quat(playerRotationReference));
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), playerPositionReference);
    GameState.cubeModel = translationMatrix * rotationMatrix;


    // Update all other objects in the GameState
    int numEntities = physicsSystem.dynamicObjects.size() + physicsSystem.staticObjects.size();
    GameState.num_objects = numEntities;
   


    //send all the dynamic objects
    for (int i = 0; i < physicsSystem.dynamicObjects.size(); i++) {
        GameObject* obj = physicsSystem.dynamicObjects[i];
        glm::vec3& position = obj->transform.position;
        glm::quat& rotation = obj->transform.rotation;
        glm::mat4 modelMatrix = physicsSystem.toMatrix(position, rotation);

        // Assuming GameState has a way to store multiple objects' model matrices
        GameState.objects[i] = modelMatrix; // Replace with actual storage logic
    }

    //send all the static objects
    for (int i = 0; i < physicsSystem.staticObjects.size(); i++) {
        GameObject* obj = physicsSystem.staticObjects[i];
        glm::vec3& position = obj->transform.position;
        glm::quat& rotation = obj->transform.rotation;
        glm::mat4 modelMatrix = physicsSystem.toMatrix(position, rotation);

        // Assuming GameState has a way to store multiple objects' model matrices
        GameState.objects[i + physicsSystem.dynamicObjects.size()] = modelMatrix; // Replace with actual storage logic
    }
}

bool ServerGame::receiveFromClients()
{
    bool receivedChanges = false;
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

        receivedChanges = true;
        printf("ServerGame::receiveFromClients received packet from %d\n", iter->first);

        int i = 0;
        while (i < (unsigned int)data_length)
        {

            //copy the network packet data into player intent
            PlayerIntent.deserialize(&(network_data[i]));

            //increment in case we have more 
            i += sizeof(PlayerIntentPacket);

            //apply the input to our game world, assume player 0 for now 
			physicsSystem.applyInput(PlayerIntent, 0); 

			////updated game state   
   //         currentTime = std::chrono::high_resolution_clock::now();

   //         if (std::chrono::duration<float>(currentTime - lastPacketSentTime).count() > 0.033f) // some fixed constant
   //         {
   //             lastPacketSentTime = currentTime;
   //             sendActionPackets();
   //         }
        }
    }

    return receivedChanges;
}

void ServerGame::sendActionPackets()
{
    // send action packet
    const unsigned int packet_size = sizeof(GameStatePacket);
    char packet_data[packet_size];

    //printf("about to send x: %f, y: %f, z: %f\n", GameState.cubeModel[3][0], GameState.cubeModel[3][1], GameState.cubeModel[3][2]);
    //printf("cube model is: ");
    //for (int j = 0; j < 4; j++) {
    //    for (int k = 0; k < 4; k++) {
    //        //printf("%f ", GameState.getModelMatrix()[i][j]);
    //        printf("%f ", GameState.cubeModel[k][j]);
    //    }
    //    printf("\n");
    //}
    //printf("server sending game state packet from server with the following mat4:");
    //for (int j = 0; j < 4; j++) {
    //    for (int k = 0; k < 4; k++) {
    //        //printf("%f ", GameState.getModelMatrix()[i][j]);
    //        printf("%f ", GameState.cubeModel[k][j]);
    //    }
    //    printf("\n");
    //}
    //printf("\n");

    GameState.serialize(packet_data);
    network->sendToAll(packet_data, packet_size);
}


