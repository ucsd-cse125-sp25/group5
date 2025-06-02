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
#include <filesystem>

#define PRE_GAME_COUNTDOWN 5
#define IN_GAME_DURATION 120
#define NUM_PLAYERS_TO_START 2

#define TICKS_PER_SECOND 100
#define TICK_TIME_MILLS (1000 / TICKS_PER_SECOND)

unsigned int ServerGame::client_id;

std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

void ServerGame::loadComposites() {
	// // Path to the composites directory  
	// const std::wstring path = L"..\\include\\shared\\composites\\*.*";  
	//
	// WIN32_FIND_DATAW findFileData;  
	// HANDLE hFind = FindFirstFileW(path.c_str(), &findFileData);  
	//
	// if (hFind == INVALID_HANDLE_VALUE) {  
	//     std::wcerr << L"FindFirstFile failed\n";  
	//     return;  
	// }  
	//
	// do {  
	//     if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {  
	//         std::wcout << L"Found file: " << findFileData.cFileName << L"\n";
	//
	//         //Load the composite here
	//         // Build full path
	//         wchar_t fullPath[MAX_PATH];
	//         swprintf(fullPath, MAX_PATH, L"..\\include\\shared\\composites\\%s", findFileData.cFileName);
	//
	//         // Open the file
	//         FILE* file = _wfopen(fullPath, L"r");
	//         if (!file) {
	//             wprintf(L"Failed to open file: %s\n", fullPath);
	//             continue;
	//         }
	//
	////remove file extension
	//std::wstring fileName = findFileData.cFileName;
	//std::wstring fileNameWithoutExt = fileName.substr(0, fileName.find_last_of(L'.'));
	// 
	//         std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	//         //create a new element for composites
	//         //std::map<std::string, std::vector<std::pair<glm::vec3, glm::vec3>>> composites;
	//         std::string s = conv.to_bytes(fileNameWithoutExt);
			 //composites[s] = std::vector<std::pair<glm::vec3, glm::vec3>>();
	// 
	//         wchar_t line[512];
	//         while (fgetws(line, sizeof(line) / sizeof(wchar_t), file)) {
	//             float values[6];
	//             int i = 0;
	//             wchar_t* context = NULL;
	//             wchar_t* token = wcstok(line, L",\n\r", &context);
	//             while (token != NULL && i < 6) {
	//                 values[i++] = (float)_wtof(token);
	//                 token = wcstok(NULL, L",\n\r", &context);
	//             }
	//             if (i == 6) {
	//                 wprintf(L"Parsed floats: %f %f %f %f %f %f\n",
	//                     values[0], values[1], values[2],
	//                     values[3], values[4], values[5]);
	//		// Store the values in the composites map
	//		glm::vec3 pos = glm::vec3(values[0], values[1], values[2]);
	//		glm::vec3 ext = glm::vec3(values[3], values[4], values[5]);
	//                 composites[s].push_back(std::make_pair(pos, ext));
	//             }
	//             else {
	//       		wprintf(L"Failed to parse line: %s\n", line);
	//             }
	//         }
	//
	//         fclose(file);
	//     }
	// } while (FindNextFileW(hFind, &findFileData) != 0);  
	//
	// FindClose(hFind);
	// 
	composites["Floating Island"] = std::vector<std::pair<glm::vec3, glm::vec3>>();
	double values[18] = { 0, 0.38, 0, 5.01, 2.64, 4.315,
							0, -4.42, 0, 3.43, 2.46, 2.845,
							-0.49, -7.66, 0, 1.96, 0.8050001, 1.73 };

	for (int i = 0; i < 3; i++) {
		glm::vec3 pos = glm::vec3(values[i * 6], values[i * 6 + 1], values[i * 6 + 2]);
		glm::vec3 ext = glm::vec3(values[i * 6 + 3], values[i * 6 + 4], values[i * 6 + 5]);
		composites["Floating Island"].push_back(std::make_pair(pos, ext));
	}

	for (int i = 0; i < composites["Floating Island"].size(); i++) {
		glm::vec3 pos = composites["Floating Island"][i].first;
		glm::vec3 ext = composites["Floating Island"][i].second;
		GameObject* col = physicsSystem.makeGameObject(pos, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), ext);
		col->type = COLLIDER;
		physicsSystem.addStaticObject(col);
	}
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
	glm::vec3 hpPickupCoordinates[3] = {
		glm::vec3(5.0f, 30.0f, 5.0f),
		glm::vec3(-5.0f, 30.0f, -5.0f),
		glm::vec3(0.0f, 30.0f, 0.0f)
	};

	glm::vec3 manaPickupCoordinates[3] = {
		glm::vec3(10.0f, 30.0f, 10.0f),
		glm::vec3(-10.0f, 30.0f, -10.0f),
		glm::vec3(15.0f, 30.0f, -15.0f)
	};
	for (int i = 0; i < 3; i++) {
		GameObject* hpPickup = physicsSystem.makeGameObject(hpPickupCoordinates[i], glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		hpPickup->type = HP_PICKUP;
		hpPickup->isDynamic = true;
		hpPickup->behavior = new PickupBehaviorComponent(hpPickup, physicsSystem, HP_PICKUP, hpPickupCoordinates[i]);
		physicsSystem.addDynamicObject(hpPickup);
		physicsSystem.addMovingObject(hpPickup);
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
	flag->transform.position = glm::vec3(5.0f, 30.0f, 0.0f);
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
	spawnIslands(physicsSystem);
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

	physicsSystem.tick(0.05f); // Update the physics system with a fixed timestep

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


