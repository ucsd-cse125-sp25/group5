#pragma once  
#include "ServerNetwork.h"  
#include "../include/shared/NetworkData.h"  
#include "physics/PhysicsData.h"
#include "physics/PhysicsSystem.h" // Include the PhysicsSystem header
#include "physics/BehaviorComponent.h"
#include "InputManager.h"
#include <chrono>
#include <map>
#include <utility>
#include <string> 

//const std::string COMPOSITES_FILE_PATH = "../include/shared/composites/";

class ServerGame {

public:  
   ServerGame(void);  
   ~ServerGame(void);  

   void update();  
   bool receiveFromClients();
   void handlePlayerIntentPacket(unsigned int id, char* buf);
   void sendGameStatePackets(); // Ensure this declaration matches the definition in ServerGame.cpp  
   void writeToGameState(); // Ensure this declaration matches the definition in ServerGame.cpp
   void loadComposites();
   void setPhase(GamePhase newPhase);
   

private:  
   static unsigned int client_id;  
   ServerNetwork* network;

   GameObject* flag = nullptr;

   GameStatePacket GameState;
   //PlayerIntentPacket PlayerIntent;  
   PhysicsSystem physicsSystem; // Add this line to include the physics system
   InputManager inputManager; // Input manager instance	

   std::map<std::string, std::vector<std::pair<glm::vec3, glm::vec3>>> composites; // Replace Map and Pair with std::map and std::pair

   PlayerBehaviorComponent* playerBehaviors[4] = { nullptr, nullptr, nullptr, nullptr }; // Array to hold player behavior components] = { nullptr, nullptr, nullptr, nullptr }; // Array of player behaviors

   GamePhase phase;
   std::chrono::time_point<std::chrono::high_resolution_clock> phaseStartTime;
};