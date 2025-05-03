#include "StdAfx.h"
#include "ServerGame.h"
#include <chrono>
#include <thread>
#include <random>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <unordered_map>

unsigned int ServerGame::client_id;

std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

unordered_map<int, int> clientToEntity;
 
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

 

    //initialization of the game state
	int numCubes = rand() % 30 + 1; // Random number between 1 and 10
 //
    for (int i = 0; i < numCubes; i++) {
		GameObject* cube = physicsSystem.makeGameObject();
		cube->transform.position = glm::vec3(rand() % 10, rand() % 10, rand() % 10);
		cube->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
        cube->type = CUBE;
		physicsSystem.addStaticObject(cube);
    }


    //add an island
	GameObject* island = physicsSystem.makeGameObject(glm::vec3(5.0f, -10.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), island_extents);
	island->type = ISLAND;
	physicsSystem.addStaticObject(island);

    //add a d_cube
	GameObject* d_cube = physicsSystem.makeGameObject();
	d_cube->transform.position = glm::vec3(5.0f, 30.0f, 0.0f);
	d_cube->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
	d_cube->type = D_CUBE;
	physicsSystem.addDynamicObject(d_cube);
    physicsSystem.addMovingObject(d_cube);

	printf("ServerGame::ServerGame created %d cubes\n", numCubes);

	//GameState.setModelMatrix(glm::mat4(1.0f)); // Initialize the cube model matrix
	//GameState.cubeModel = glm::mat4(1.0f); // Initialize the cube model matrix
 //   physicsSystem.playerObjects[0] = cube;
}

ServerGame::~ServerGame(void)
{
}

void PrintPlayerIntent(const PlayerIntentPacket& intent) {
    //printf("PlayerIntentPacket: \n");
    //printf("moveLeftIntent: %d\n", intent.moveLeftIntent);
    //printf("moveRightIntent: %d\n", intent.moveRightIntent);
    //printf("moveUpIntent: %d\n", intent.moveUpIntent);
    //printf("moveDownIntent: %d\n", intent.moveDownIntent);
    //printf("moveForwardIntent: %d\n", intent.moveForwardIntent);
    //printf("moveBackIntent: %d\n", intent.moveBackIntent);
    //printf("azimuthIntent: %f\n", intent.azimuthIntent);
    //printf("inclineIntent: %f\n", intent.inclineIntent);
    //printf("rightClickIntent: %d\n", intent.rightClickIntent);
    //printf("leftClickIntent: %d\n", intent.leftClickIntent);
    printf("scrollUpIntent: %d\n", intent.scrollUpIntent);
    printf("scrollDownIntent: %d\n", intent.scrollDownIntent);
}

void ServerGame::update()
{
    startTime = std::chrono::high_resolution_clock::now();

    // get new clients
   if(network->acceptNewClient(client_id))
   {
        printf("client %d has been connected to the server\n",client_id);
        GameObject* player = physicsSystem.makeGameObject();
		player->type = PLAYER;
        //place where player gets added
        //physicsSystem.playerObjects[client_id] = player;
		physicsSystem.addPlayerObject(player);
        physicsSystem.addMovingObject(player);
        player->id = client_id;
        //physicsSystem.addDynamicObject(player);
        clientToEntity[client_id] = client_id;
        
        //fill up the HP and the mana


        JoinResponsePacket packet;
        packet.packet_type = JOIN_RESPONSE;
        packet.entity_id = player->id;
        const unsigned int packet_size = sizeof(JoinResponsePacket);
        char packet_data[packet_size];
        packet.serialize(packet_data);
        network->sendTo(client_id, packet_data, packet_size);
        client_id++;
   }

   bool sendUpdate = receiveFromClients();

   physicsSystem.tick(0.05f); // Update the physics system with a fixed timestep
   //put new information into the game state
   writeToGameState();


   if (sendUpdate) {
       sendGameStatePackets();
   }

   endTime = std::chrono::high_resolution_clock::now();
   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

   if (duration < 50) // If did not spend the whole tick (50ms)
   {
       std::this_thread::sleep_for(std::chrono::milliseconds(duration));
   }
}


void ServerGame::writeToGameState() {
    GameState.packet_type = GAME_STATE;

    // Update all other objects in the GameState
    int numEntities = physicsSystem.dynamicObjects.size() + physicsSystem.staticObjects.size();
    GameState.num_entities = numEntities;
	int numPlayers = physicsSystem.playerObjects.size();
	GameState.num_players = numPlayers;

    //send all the player objects, probably want to do this differently at some point, lock the correspondance between playerID and arrayIndex
    for (int i = 0; i < physicsSystem.playerObjects.size(); i++) {
        GameObject* obj = physicsSystem.playerObjects[i];
        glm::vec3& position = obj->transform.position;
        glm::quat& rotation = obj->transform.rotation;
        glm::mat4 modelMatrix = physicsSystem.toMatrix(position, rotation);
        // Assuming GameState has a way to store multiple objects' model matrices
        GameState.players[i] = Entity{ (unsigned int)obj->id, obj->type, modelMatrix };
    }
   
    //send all the dynamic objects
    for (int i = 0; i < physicsSystem.dynamicObjects.size(); i++) {
        GameObject* obj = physicsSystem.dynamicObjects[i];
        glm::vec3& position = obj->transform.position;
        glm::quat& rotation = obj->transform.rotation;
        glm::mat4 modelMatrix = physicsSystem.toMatrix(position, rotation);
        // Assuming GameState has a way to store multiple objects' model matrices
        GameState.entities[i] = Entity{ (unsigned int) obj->id, obj->type, modelMatrix };

		printf("ServerGame::writeToGameState sending entity %d with type %d\n", obj->id, obj->type);
		printf("position is %f %f %f\n", position.x, position.y, position.z);
    }

    //send all the static objects
    for (int i = 0; i < physicsSystem.staticObjects.size(); i++) {
        GameObject* obj = physicsSystem.staticObjects[i];
        glm::vec3& position = obj->transform.position;
        glm::quat& rotation = obj->transform.rotation;
        glm::mat4 modelMatrix = physicsSystem.toMatrix(position, rotation);

        // Assuming GameState has a way to store multiple objects' model matrices
        GameState.entities[i + physicsSystem.dynamicObjects.size()] = Entity{ (unsigned int) obj->id, obj->type, modelMatrix };

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

            //apply the input to our game world
			physicsSystem.applyInput(PlayerIntent, clientToEntity[iter->first]); 
        }
    }

    return receivedChanges;
}

void ServerGame::sendGameStatePackets()
{
    // send action packet
    const unsigned int packet_size = sizeof(GameStatePacket);
    char packet_data[packet_size];


	printf("Sending num_entities : %d\n", GameState.num_entities);
	printf("First entity type: %d\n", GameState.entities[0].type);

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


