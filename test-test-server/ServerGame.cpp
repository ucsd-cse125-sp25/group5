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
#include <algorithm>
#include <windows.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1;
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#define PRE_GAME_COUNTDOWN 10

#define IN_GAME_DURATION 300

#define NUM_PLAYERS_TO_START 1   



#define TICKS_PER_SECOND 50
#define TICK_TIME_MILLS (1000 / TICKS_PER_SECOND)

unsigned int ServerGame::client_id;

std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

#include <fstream> // Add this include to resolve incomplete type "std::ifstream"

void ServerGame::loadComposites() {
    string PATH_TO_COLLIDERS = "../../../colliders/";

    for (const auto& entry : experimental::filesystem::directory_iterator(PATH_TO_COLLIDERS)) {
        const fs::path& file_path = entry.path();

        std::cout << "Reading file: " << entry.path().stem() << "\n";

        // Initialize the ifstream object properly
        std::ifstream in(file_path.string());

        if (!in) {
            std::cout << "Failed to open " << file_path << "\n";
			string file_name = entry.path().stem().string();
			composites[file_name] = std::vector<std::pair<glm::vec3, glm::vec3>>();

            continue;
        }

        string file_name = entry.path().stem().string();
        composites[file_name] = std::vector<std::pair<glm::vec3, glm::vec3>>();

        int num_colliders;
        in >> num_colliders;

        for (int i = 0; i < num_colliders; i++) {
            float pX, pY, pZ;
            float eX, eY, eZ;
            in >> pX >> pY >> pZ >> eX >> eY >> eZ;
            glm::vec3 pos = glm::vec3(pX, pY, pZ);
            glm::vec3 ext = glm::vec3(eX, eY, eZ);
            composites[file_name].push_back(std::make_pair(pos, ext));
            //GameObject* col = physicsSystem.makeGameObject(pos, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), ext);
            //col->type = COLLIDER;
            //physicsSystem.addStaticObject(col);
        }
    }

	//add map objects
	for (int i = 0; i < mapObjects.size(); i++) {
		auto entry = mapObjects[i];
		//type of object to have collider, and the position


		std::string file_name = std::get<0>(entry);
		glm::vec3 position = std::get<1>(entry);

		// pray to god that the composites has that file, and then generate all the colliders at the proper position
		for(int j = 0; j < composites[file_name].size(); j++) {
			glm::vec3 pos = composites[file_name][j].first + position;
			glm::vec3 ext = composites[file_name][j].second;
			GameObject* col = physicsSystem.makeGameObject(pos, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), ext);
			col->type = COLLIDER;
			physicsSystem.addStaticObject(col);
		}
	}

    printf("Finished loading colliders.\n");
}

void spawnIslands(PhysicsSystem& physicsSystem) {
	glm::vec3 islandCoordinates[7] = {
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(10.0f, 10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, -10.0f),
		glm::vec3(20.0f, 20.0f, 20.0f),
		glm::vec3(-20.0f, -20.0f, -20.0f),
		glm::vec3(10.0f, -10.0f, -10.0f),
		glm::vec3(-10.0f, 10.0f, 10.0f),

	};

	for (int i = 0; i < 7; i++) {
		GameObject* island = physicsSystem.makeGameObject(islandCoordinates[i] * 2.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), island_extents);
		island->type = ISLAND;
		physicsSystem.addStaticObject(island);
	}

	GameObject* ground = physicsSystem.makeGameObject(glm::vec3(0.0f, -20.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), groundExtents);
	ground->type = COLLIDER;
	physicsSystem.addStaticObject(ground);


}


void spawnPickups(PhysicsSystem& physicsSystem) {
	const int numHPickups = 105; // Number of health pickups
	glm::vec3 hpPickupCoordinates[numHPickups] = {
		glm::vec3(0.000f,37.000f,13.900f),
glm::vec3(-38.700f,40.590f,-0.350f),
glm::vec3(-13.100f,46.630f,-28.040f),
glm::vec3(-2.700f,50.630f,-7.060f),
glm::vec3(38.800f,53.200f,-12.350f),
glm::vec3(31.800f,56.700f,3.600f),
glm::vec3(21.000f,55.300f,-22.790f),
glm::vec3(17.220f,51.470f,-7.650f),
glm::vec3(-26.200f,49.300f,22.800f),
glm::vec3(-17.020f,44.700f,29.400f),
glm::vec3(-42.020f,40.120f,21.500f),
glm::vec3(-19.100f,36.620f,14.800f),
glm::vec3(0.000f,100.130f,13.300f),
glm::vec3(-35.800f,103.720f,-0.350f),
glm::vec3(-13.100f,106.000f,-25.100f),
glm::vec3(-10.700f,113.760f,-7.060f),
glm::vec3(15.600f,114.600f,-6.840f),
glm::vec3(21.000f,117.200f,-22.790f),
glm::vec3(31.800f,119.830f,3.600f),
glm::vec3(-19.100f,99.750f,10.400f),
glm::vec3(-42.020f,103.250f,23.600f),
glm::vec3(-17.020f,107.830f,27.500f),
glm::vec3(-26.200f,120.600f,22.800f),
glm::vec3(41.300f,108.600f,-12.350f),
glm::vec3(38.780f,65.200f,-15.520f),
glm::vec3(31.346f,67.499f,-12.889f),
glm::vec3(23.913f,69.798f,-10.258f),
glm::vec3(16.479f,72.097f,-7.627f),
glm::vec3(9.045f,74.396f,-4.996f),
glm::vec3(1.612f,76.695f,-2.365f),
glm::vec3(-5.822f,78.993f,0.265f),
glm::vec3(-13.255f,81.292f,2.896f),
glm::vec3(-20.689f,83.591f,5.527f),
glm::vec3(-28.123f,85.890f,8.158f),
glm::vec3(-35.556f,88.189f,10.789f),
glm::vec3(-42.990f,90.488f,13.420f),
glm::vec3(42.042f,137.400f,-67.242f),
glm::vec3(34.542f,146.999f,-69.340f),
glm::vec3(24.505f,152.931f,-65.235f),
glm::vec3(15.765f,152.931f,-56.495f),
glm::vec3(11.660f,146.999f,-46.458f),
glm::vec3(13.758f,137.400f,-38.958f),
glm::vec3(21.258f,127.802f,-36.860f),
glm::vec3(31.295f,121.869f,-40.964f),
glm::vec3(40.036f,121.869f,-49.705f),
glm::vec3(44.141f,127.802f,-59.742f),
glm::vec3(36.501f,48.300f,13.719f),
glm::vec3(29.000f,57.898f,11.621f),
glm::vec3(18.963f,63.831f,15.726f),
glm::vec3(10.223f,63.831f,24.466f),
glm::vec3(6.118f,57.898f,34.503f),
glm::vec3(8.216f,48.300f,42.003f),
glm::vec3(15.716f,38.702f,44.102f),
glm::vec3(25.754f,32.769f,39.997f),
glm::vec3(34.494f,32.769f,31.257f),
glm::vec3(38.599f,38.702f,21.219f),
glm::vec3(15.742f,82.600f,-71.342f),
glm::vec3(8.242f,92.198f,-73.440f),
glm::vec3(-1.795f,98.131f,-69.335f),
glm::vec3(-10.536f,98.131f,-60.595f),
glm::vec3(-14.641f,92.198f,-50.558f),
glm::vec3(-12.542f,82.600f,-43.058f),
glm::vec3(-5.042f,73.002f,-40.959f),
glm::vec3(4.995f,67.069f,-45.064f),
glm::vec3(13.735f,67.069f,-53.805f),
glm::vec3(17.840f,73.002f,-63.842f),
glm::vec3(-37.558f,60.200f,-14.142f),
glm::vec3(-45.058f,69.798f,-16.240f),
glm::vec3(-55.095f,75.731f,-12.136f),
glm::vec3(-63.836f,75.731f,-3.395f),
glm::vec3(-67.940f,69.798f,6.642f),
glm::vec3(-65.842f,60.200f,14.142f),
glm::vec3(-58.342f,50.602f,16.240f),
glm::vec3(-48.305f,44.669f,12.136f),
glm::vec3(-39.565f,44.669f,3.395f),
glm::vec3(-35.460f,50.602f,-6.642f),
glm::vec3(-11.390f,127.160f,10.000f),
glm::vec3(-14.779f,131.077f,4.154f),
glm::vec3(-22.393f,132.044f,2.711f),
glm::vec3(-28.499f,129.334f,6.756f),
glm::vec3(-28.499f,124.986f,13.244f),
glm::vec3(-22.393f,122.276f,17.289f),
glm::vec3(-14.779f,123.243f,15.846f),
glm::vec3(51.100f,140.450f,44.600f),
glm::vec3(47.885f,152.450f,44.600f),
glm::vec3(39.100f,161.235f,44.600f),
glm::vec3(27.100f,164.450f,44.600f),
glm::vec3(15.100f,161.235f,44.600f),
glm::vec3(6.315f,152.450f,44.600f),
glm::vec3(3.100f,140.450f,44.600f),
glm::vec3(6.315f,128.450f,44.600f),
glm::vec3(15.100f,119.665f,44.600f),
glm::vec3(27.100f,116.450f,44.600f),
glm::vec3(39.100f,119.665f,44.600f),
glm::vec3(47.885f,128.450f,44.600f),
glm::vec3(0.000f,4.100f,20.000f),
glm::vec3(11.756f,4.100f,16.180f),
glm::vec3(19.021f,4.100f,6.180f),
glm::vec3(19.021f,4.100f,-6.180f),
glm::vec3(11.756f,4.100f,-16.180f),
glm::vec3(0.000f,4.100f,-20.000f),
glm::vec3(-11.756f,4.100f,-16.180f),
glm::vec3(-19.021f,4.100f,-6.180f),
glm::vec3(-19.021f,4.100f,6.180f),
glm::vec3(-11.756f,4.100f,16.180f),
	};

	const int numManaPickups = 89;

	glm::vec3 manaPickupCoordinates[numManaPickups] = {
		glm::vec3(27.900f,139.420f,-49.100f),
glm::vec3(31.364f,139.420f,-51.100f),
glm::vec3(31.364f,139.420f,-55.100f),
glm::vec3(27.900f,139.420f,-57.100f),
glm::vec3(24.436f,139.420f,-55.100f),
glm::vec3(24.436f,139.420f,-51.100f),
glm::vec3(22.358f,50.320f,31.861f),
glm::vec3(25.823f,50.320f,29.861f),
glm::vec3(25.823f,50.320f,25.861f),
glm::vec3(22.358f,50.320f,23.861f),
glm::vec3(18.894f,50.320f,25.861f),
glm::vec3(18.894f,50.320f,29.861f),
glm::vec3(1.600f,84.620f,-53.200f),
glm::vec3(5.064f,84.620f,-55.200f),
glm::vec3(5.064f,84.620f,-59.200f),
glm::vec3(1.600f,84.620f,-61.200f),
glm::vec3(-1.864f,84.620f,-59.200f),
glm::vec3(-1.864f,84.620f,-55.200f),
glm::vec3(-51.700f,62.220f,4.000f),
glm::vec3(-48.236f,62.220f,2.000f),
glm::vec3(-48.236f,62.220f,-2.000f),
glm::vec3(-51.700f,62.220f,-4.000f),
glm::vec3(-55.164f,62.220f,-2.000f),
glm::vec3(-55.164f,62.220f,2.000f),
glm::vec3(0.000f,3.560f,1.000f),
glm::vec3(0.866f,3.560f,-0.500f),
glm::vec3(-0.866f,3.560f,-0.500f),
glm::vec3(-20.390f,127.160f,13.000f),
glm::vec3(-17.537f,127.160f,10.927f),
glm::vec3(-18.627f,127.160f,7.573f),
glm::vec3(-22.153f,127.160f,7.573f),
glm::vec3(-23.243f,127.160f,10.927f),
glm::vec3(-12.200f,25.800f,-10.648f),
glm::vec3(-11.334f,25.800f,-12.148f),
glm::vec3(-13.066f,25.800f,-12.148f),
glm::vec3(27.100f,140.760f,49.600f),
glm::vec3(30.636f,140.760f,48.136f),
glm::vec3(32.100f,140.760f,44.600f),
glm::vec3(30.636f,140.760f,41.064f),
glm::vec3(27.100f,140.760f,39.600f),
glm::vec3(23.564f,140.760f,41.064f),
glm::vec3(22.100f,140.760f,44.600f),
glm::vec3(23.564f,140.760f,48.136f),
glm::vec3(-2.300f,27.160f,3.460f),
glm::vec3(-1.434f,27.160f,1.960f),
glm::vec3(-3.166f,27.160f,1.960f),
glm::vec3(3.940f,34.050f,-26.500f),
glm::vec3(4.806f,34.050f,-28.000f),
glm::vec3(3.074f,34.050f,-28.000f),
glm::vec3(0.000f,4.100f,16.000f),
glm::vec3(9.405f,4.100f,12.944f),
glm::vec3(15.217f,4.100f,4.944f),
glm::vec3(15.217f,4.100f,-4.944f),
glm::vec3(9.405f,4.100f,-12.944f),
glm::vec3(0.000f,4.100f,-16.000f),
glm::vec3(-9.405f,4.100f,-12.944f),
glm::vec3(-15.217f,4.100f,-4.944f),
glm::vec3(-15.217f,4.100f,4.944f),
glm::vec3(-9.405f,4.100f,12.944f),
glm::vec3(6.100f,94.500f,-33.100f),
glm::vec3(6.966f,94.500f,-34.600f),
glm::vec3(5.234f,94.500f,-34.600f),
glm::vec3(-15.738f,46.990f,37.007f),
glm::vec3(-15.826f,46.990f,37.003f),
glm::vec3(-15.913f,46.990f,37.001f),
glm::vec3(-16.000f,46.990f,37.000f),
glm::vec3(-16.087f,46.990f,37.001f),
glm::vec3(-16.175f,46.990f,37.003f),
glm::vec3(-45.960f,92.200f,14.500f),
glm::vec3(-45.094f,92.200f,13.000f),
glm::vec3(-46.826f,92.200f,13.000f),
glm::vec3(8.000f,92.200f,39.700f),
glm::vec3(8.866f,92.200f,38.200f),
glm::vec3(7.134f,92.200f,38.200f),
glm::vec3(15.150f,32.800f,-8.920f),
glm::vec3(16.016f,32.800f,-10.420f),
glm::vec3(14.284f,32.800f,-10.420f),
glm::vec3(-6.100f,35.200f,-13.170f),
glm::vec3(-5.234f,35.200f,-14.670f),
glm::vec3(-6.966f,35.200f,-14.670f),
glm::vec3(-51.248f,62.750f,-4.993f),
glm::vec3(-51.336f,62.750f,-4.997f),
glm::vec3(-51.423f,62.750f,-4.999f),
glm::vec3(-51.510f,62.750f,-5.000f),
glm::vec3(-51.597f,62.750f,-4.999f),
glm::vec3(-51.684f,62.750f,-4.997f),
glm::vec3(-27.920f,100.100f,-47.500f),
glm::vec3(-27.054f,100.100f,-49.000f),
glm::vec3(-28.786f,100.100f,-49.000f),



	};
	for (int i = 0; i < numHPickups; i++) {
		GameObject* hpPickup = physicsSystem.makeGameObject(hpPickupCoordinates[i], glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		hpPickup->type = HP_PICKUP;
		hpPickup->isDynamic = true;
		hpPickup->behavior = new PickupBehaviorComponent(hpPickup, physicsSystem, HP_PICKUP, hpPickupCoordinates[i]);
		physicsSystem.addDynamicObject(hpPickup);
		physicsSystem.addMovingObject(hpPickup);
	}	

	for(int i = 0; i < numManaPickups; i++) {
		GameObject* manaPickup = physicsSystem.makeGameObject(manaPickupCoordinates[i], glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		manaPickup->type = MANA_PICKUP;
		manaPickup->isDynamic = true;
		manaPickup->behavior = new PickupBehaviorComponent(manaPickup, physicsSystem, MANA_PICKUP, manaPickupCoordinates[i]);
		physicsSystem.addDynamicObject(manaPickup);
		physicsSystem.addMovingObject(manaPickup);
	}
}

GameObject* spawnFlag(PhysicsSystem& physicsSystem) {
	GameObject* flag = physicsSystem.makeGameObject();
	flag->transform.position = glm::vec3(0.0f, 10.0f, 0.0f);
	flag->transform.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
	flag->type = FLAG;
	flag->behavior = new FlagBehaviorComponent(flag, physicsSystem);
	flag->isDynamic = true;
	physicsSystem.addDynamicObject(flag);
	physicsSystem.addMovingObject(flag);
	return flag;
}

ServerGame::ServerGame(void) {
	// ids to assign clients for our table
	client_id = 0;

	// set up the server network to listen 
	network = new ServerNetwork();

	// the current game state TO SEND (not necessarily full game state)
	GameState = GameStatePacket();

	//input management
	inputManager = InputManager();

	//initialize the physics system
	physicsSystem = PhysicsSystem();

	// load fixed colliders for the map
	loadComposites();

	//add islands
	//spawnIslands(physicsSystem);
	
	spawnPickups(physicsSystem);

	//add a flag
	flag = spawnFlag(physicsSystem);

	setPhase(WAITING);
}

ServerGame::~ServerGame(void) {}

void ServerGame::setPhase(GamePhase newPhase) {
	phase = newPhase;
	phaseStartTime = std::chrono::high_resolution_clock::now();
}

void ServerGame::update() {
	startTime = std::chrono::high_resolution_clock::now();

	// get new clients
	if (network->acceptNewClient(client_id)) {
		// create a new player
		GameObject* player = physicsSystem.makeGameObject();

		//take care of behavior stuff
		PlayerBehaviorComponent* playerBehavior = new PlayerBehaviorComponent(player, physicsSystem); //for player objects, we set a behavior component
		player->behavior = playerBehavior;
		playerBehaviors[client_id] = playerBehavior;
		//for player objects, we set a behavior component
		printf("Metal mana %d\n", playerBehavior->playerStats.mana[0]);
		printf("Wood mana %d\n", playerBehavior->playerStats.mana[1]);
		printf("Water mana %d\n", playerBehavior->playerStats.mana[2]);
		printf("Fire mana %d\n", playerBehavior->playerStats.mana[3]);
		printf("Earth mana %d\n", playerBehavior->playerStats.mana[4]);

		player->type = PLAYER;
		player->isDynamic = true;
		player->transform.position = spawnPoints[client_id];
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

		if (phase == WAITING && physicsSystem.playerObjects.size() == NUM_PLAYERS_TO_START) {
			setPhase(PRE_GAME);
		}
	}

	bool sendUpdate = receiveFromClients();
	
	physicsSystem.tick(0.1f); // Update the physics system with a fixed timestep

	std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
	int timeSinceStart = (int)std::chrono::duration<float>(now - ServerGame::phaseStartTime).count();

	if (phase == PRE_GAME && PRE_GAME_COUNTDOWN - timeSinceStart <= 0) {
		setPhase(IN_GAME);
	}

	writeToGameState();

	if (sendUpdate) {
		sendGameStatePackets();
	}

	endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	// If did not spend the whole tick (50ms)
	if (duration < TICK_TIME_MILLS) {
		std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME_MILLS - duration));
	}
}

void writeEntities(PhysicsSystem& physicsSystem, std::vector<GameObject*>& objects, Entity* lst, unsigned int startIndex, unsigned int endIndex) {
	if (objects.empty()) {
		return; // No objects to send
	}

	unsigned int j = 0;

	for (unsigned int i = startIndex; i < endIndex; i++) {
		glm::mat4 modelMatrix = physicsSystem.toMatrix(objects[j]->transform.position, objects[j]->transform.rotation);
		lst[i] = Entity{ (unsigned int)objects[j]->id, objects[j]->type, modelMatrix, objects[j]->collider->halfExtents };
		j++;
	}
}

void writeKillfeed(PhysicsSystem& phys, std::vector<KillfeedItem> killfeed_queue, GameStatePacket& gameState) {
	for (int i = 0; i < (((killfeed_queue.size()) < (KILLFEED_LENGTH)) ? (killfeed_queue.size()) : (KILLFEED_LENGTH)); i++) {
		gameState.killfeed[i] = killfeed_queue[i];
	}
}

void ServerGame::writeToGameState() {
	GameState.packet_type = GAME_STATE;

	// Update all other objects in the GameState
	unsigned int numEntities = physicsSystem.dynamicObjects.size() + physicsSystem.staticObjects.size();
	GameState.num_entities = numEntities;
	unsigned int numPlayers = physicsSystem.playerObjects.size();
	GameState.num_players = numPlayers;

	GameState.phase = phase;

	std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
	float timeSinceStart = std::chrono::duration<float>(now - ServerGame::phaseStartTime).count();
	
	switch (phase) {
		case PRE_GAME:
			GameState.time = PRE_GAME_COUNTDOWN - (int) timeSinceStart;
			break;
		case IN_GAME:
			GameState.time = IN_GAME_DURATION - (int) timeSinceStart;
			break;
		default:
			GameState.time = (int) timeSinceStart;
	}

	physicsSystem.timePassed = timeSinceStart; // Update the physics system with the time passed since the start of the game
	physicsSystem.totalTime = IN_GAME_DURATION; // Set the total time for the physics system
	//GameState.moonPhase = static_cast<MoonPhase>(timeLeft / IN_GAME_DURATION * 5); // Assuming 4 phases of the moon, this will cycle through them based on time left
	if(physicsSystem.timePassed < 150.0f) {
		GameState.moonPhase = NEW_MOON;
	} else if(physicsSystem.timePassed < 240.0f) {
		GameState.moonPhase = FIRST_QUARTER;
	} else {
		GameState.moonPhase = FULL_MOON;
	}


	//send all the player objects, probably want to do this differently at some point, lock the correspondance between playerID and arrayIndex
	writeEntities(physicsSystem, physicsSystem.playerObjects, GameState.players, 0, numPlayers);

	//send all the dynamic objects
	writeEntities(physicsSystem, physicsSystem.dynamicObjects, GameState.entities, 0, physicsSystem.dynamicObjects.size());

	//send all the static objects
	writeEntities(physicsSystem, physicsSystem.staticObjects, GameState.entities, physicsSystem.dynamicObjects.size(), numEntities);

	//send the killfeed
	writeKillfeed(physicsSystem, physicsSystem.killfeed_queue, GameState);
  
	GameState.waterLevel = physicsSystem.waterLevel; // Update the water level in the game state

	for (int i = 0; i < 4; i++) {
		if (playerBehaviors[i] != nullptr) {
			GameState.player_stats[i] = playerBehaviors[i]->playerStats;
		}

		//check for winners

		//time has run out and we still have no winner
		if (phase == IN_GAME && GameState.time <= 0 && GameState.lockedWinnerId == -1) {
			//one of the players has the flag
			if (playerBehaviors[i] != nullptr && playerBehaviors[i]->playerStats.hasFlag) {
				setPhase(POST_GAME);
				GameState.lockedWinnerId = i;
				printf("Player %d has won\n", GameState.lockedWinnerId);
				physicsSystem.waterLevel = -2.0f; // Reset the water level to 0 when a player wins
			}
		}
	}
}

struct {
	char rem[MAX_PACKET_SIZE];
	int remSize = 0;
	PacketTypes type = NONE;
} bufs[4];

char network_data[MAX_PACKET_SIZE];

void ServerGame::handlePlayerIntentPacket(unsigned int id, char* buf) {
	physicsSystem.PlayerIntents[id].deserialize(buf);
	physicsSystem.applyInput(physicsSystem.PlayerIntents[id], id);
	inputManager.updateTracking(physicsSystem.PlayerIntents[id], id);
	physicsSystem.PlayerTrackings[id] = inputManager.playerIntentTrackers[id];
}

bool ServerGame::receiveFromClients() {
	bool receivedChanges = false;

	// go through all clients
	std::map<unsigned int, SOCKET>::iterator iter;

	for (iter = network->sessions.begin(); iter != network->sessions.end(); iter++) {
		unsigned int id = iter->first;
		int bytes_received = network->receiveData(id, network_data);

		if (bytes_received >= 0) {
			receivedChanges = true;
			int i = 0;

			// There is partial packet stored in rem
			if (bufs[id].type != NONE) {
				int bytes_required = sizeof(PlayerIntentPacket) - bufs[id].remSize;

				// buffer STILL has less bytes than what's needed for processing a full GameState Packet
				if (bytes_received < bytes_required) {
					memcpy(&(bufs[id].rem[bufs[id].remSize]), network_data, bytes_received);
					bufs[id].remSize += bytes_received;
					bufs[id].type = PLAYER_INTENT;
					i += bytes_received;
				}
				else {
					memcpy(&(bufs[id].rem[bufs[id].remSize]), network_data, bytes_required);
					handlePlayerIntentPacket(id, bufs[id].rem);
					bufs[id].remSize = 0;
					bufs[id].type = NONE;
					i += bytes_required;
				}
			}

			while (i < bytes_received) {
				// GameState Packet
				int bytes_required = sizeof(PlayerIntentPacket);
				int bytes_remaining = bytes_received - i;

				// buffer has less bytes than what's needed for processing a full GameState Packet
				if (bytes_remaining < bytes_required) {
					memcpy(bufs[id].rem, &(network_data[i]), bytes_remaining);
					bufs[id].remSize = bytes_remaining;
					bufs[id].type = PLAYER_INTENT;
					break;
				}

				handlePlayerIntentPacket(id, &(network_data[i]));
				i += sizeof(PlayerIntentPacket);
			}
		}
	}

	return receivedChanges;
}

void ServerGame::sendGameStatePackets() {
	// send action packet
	const unsigned int packet_size = sizeof(GameStatePacket);
	char packet_data[packet_size];

	GameState.serialize(packet_data);
	network->sendToAll(packet_data, packet_size);
}


