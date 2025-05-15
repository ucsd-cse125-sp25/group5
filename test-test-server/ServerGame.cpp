#include "StdAfx.h"
#include "ServerGame.h"
#include <chrono>
#include <thread>
#include <random>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include "physics/BehaviorComponent.h"
#include "../include/shared/ObjectData.h"
#include "../include/shared/NetworkData.h"

#define TICKS_PER_SECOND 100
#define TICK_TIME_MILLS (1000 / TICKS_PER_SECOND)

unsigned int ServerGame::client_id;

std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

GameStatePacket GameState;

void spawnIslands(PhysicsSystem& physicsSystem) {
	glm::vec3 islandCoordinates[7] = {
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(10.0f, 10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, -10.0f),
		glm::vec3(20.0f, 20.0f, 20.0f),
		glm::vec3(-20.0f, -20.0f, -20.0f),
		glm::vec3(10.0f, -10.0f, -10.0f),
		glm::vec3(-10.0f, 10.0f, 10.0f)
	};

	for (int i = 0; i < 7; i++) {
		GameObject* island = physicsSystem.makeGameObject(islandCoordinates[i] * 2.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), island_extents);
		island->type = ISLAND;
		physicsSystem.addStaticObject(island);
	}
}


ServerGame::ServerGame(void)
{
    // ids to assign clients for our table
    client_id = 0;

    // set up the server network to listen 
    network = new ServerNetwork();

    // the current game state TO SEND (not necessarily full game state)
	GameState = GameStatePacket();

	//the current player intent received
	//PlayerIntent = PlayerIntentPacket();

    // initialization of the game state
	// int numCubes = rand() % 30 + 1; // Random number between 1 and 10
    //input management
    inputManager = InputManager();

    //initialize the physics system
    physicsSystem = PhysicsSystem();
 
    // create a random number of cubes which are static game objects
    // for (int i = 0; i < numCubes; i++) {
	// 	GameObject* cube = physicsSystem.makeGameObject();
	// 	cube->transform.position = glm::vec3(rand() % 10, rand() % 10, rand() % 10);
	// 	cube->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
    //     cube->type = CUBE;
	// 	physicsSystem.addStaticObject(cube);
    // }
    
    //add islands
	spawnIslands(physicsSystem);

    //add a flag
	flag = physicsSystem.makeGameObject();
	flag->transform.position = glm::vec3(5.0f, 30.0f, 0.0f);
	flag->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
	flag->type = FLAG;
    flag->behavior = new FlagBehaviorComponent(flag, physicsSystem);
    flag->isDynamic = true;
	physicsSystem.addDynamicObject(flag);
    physicsSystem.addMovingObject(flag);

	//printf("ServerGame::ServerGame created %d cubes\n", numCubes);

	//GameState.setModelMatrix(glm::mat4(1.0f)); // Initialize the cube model matrix
	//GameState.cubeModel = glm::mat4(1.0f); // Initialize the cube model matrix
 //   physicsSystem.playerObjects[0] = cube;
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
        // create a new player
        GameObject* player = physicsSystem.makeGameObject();

        //take care of behavior stuff
		PlayerBehaviorComponent* playerBehavior = new PlayerBehaviorComponent(player, physicsSystem); //for player objects, we set a behavior component
        player->behavior = playerBehavior;
        playerBehaviors[client_id] = playerBehavior;
		//for player objects, we set a behavior component

		player->type = PLAYER;
        player->isDynamic = true;
        //place where player gets added
        //physicsSystem.playerObjects[client_id] = player;
		physicsSystem.addPlayerObject(player);
        physicsSystem.addMovingObject(player);
        player->id = client_id;
        //physicsSystem.addDynamicObject(player);

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
   //inputManager.updateTracking(PlayerIntent, client_id);
   writeToGameState();

   if (sendUpdate) {
       sendGameStatePackets();
   }

   endTime = std::chrono::high_resolution_clock::now();
   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

   if (duration < TICK_TIME_MILLS) // If did not spend the whole tick (50ms)
   {
       std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME_MILLS - duration));
   }
}

void writeEntities(PhysicsSystem & physicsSystem, std::vector<GameObject*>& objects, Entity* lst, unsigned int startIndex, unsigned int endIndex) {
    if (objects.empty()) {
        return; // No objects to send
    }

    unsigned int j = 0;

    for (unsigned int i = startIndex; i < endIndex; i++) {
        glm::mat4 modelMatrix = physicsSystem.toMatrix(objects[j]->transform.position, objects[j]->transform.rotation);
        lst[i] = Entity{ (unsigned int)objects[j]->id, objects[j]->type, modelMatrix };
        j++;
    }
}

void ServerGame::writeToGameState() {
    GameState.packet_type = GAME_STATE;

    // Update all other objects in the GameState
    unsigned int numEntities = physicsSystem.dynamicObjects.size() + physicsSystem.staticObjects.size();
    GameState.num_entities = numEntities;
	unsigned int numPlayers = physicsSystem.playerObjects.size();
	GameState.num_players = numPlayers;

    //send all the player objects, probably want to do this differently at some point, lock the correspondance between playerID and arrayIndex
    writeEntities(physicsSystem, physicsSystem.playerObjects, GameState.players, 0, numPlayers);
   
    //send all the dynamic objects
    writeEntities(physicsSystem, physicsSystem.dynamicObjects, GameState.entities, 0, physicsSystem.dynamicObjects.size());

    //send all the static objects
    writeEntities(physicsSystem, physicsSystem.staticObjects, GameState.entities, physicsSystem.dynamicObjects.size(), numEntities);

    for (int i = 0; i < 4; i++)
	{
        if (playerBehaviors[i] != nullptr) {
            GameState.player_stats[i] = playerBehaviors[i]->playerStats;
        }
	}
}

bool ServerGame::receiveFromClients()
{
    bool receivedChanges = false;

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

        unsigned int i = 0;
        while (i < (unsigned int)data_length)
        {

            //copy the network packet data into player intent
            //PlayerIntent.deserialize(&(network_data[i]));
			physicsSystem.PlayerIntents[iter->first].deserialize(&(network_data[i]));

      //increment in case we have more 
      i += sizeof(PlayerIntentPacket);

            //apply the input to our game world
			physicsSystem.applyInput(physicsSystem.PlayerIntents[iter->first], iter->first);
            inputManager.updateTracking(physicsSystem.PlayerIntents[iter->first], iter->first);
			physicsSystem.PlayerTrackings[iter->first] = inputManager.playerIntentTrackers[iter->first];
			//print the player intent
			//PrintPlayerIntent(PlayerIntent);
			//printf("ServerGame::receiveFromClients received packet from %d\n", iter->first);
			//printf("ServerGame::receiveFromClients received packet of type %d\n", PlayerIntent.packet_type);
			//printf("ServerGame::receiveFromClients received packet of size %d\n", data_length);

        }
    }

    return receivedChanges;
}

void ServerGame::sendGameStatePackets()
{
    // send action packet
    const unsigned int packet_size = sizeof(GameStatePacket);
    char packet_data[packet_size];

    GameState.serialize(packet_data);
    network->sendToAll(packet_data, packet_size);
}


