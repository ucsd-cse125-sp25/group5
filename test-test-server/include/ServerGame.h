#pragma once  
#include "ServerNetwork.h"  
#include "../include/shared/NetworkData.h"  
#include "physics/PhysicsData.h"
#include "physics/PhysicsSystem.h" // Include the PhysicsSystem header
#include "physics/BehaviorComponent.h"

class ServerGame  
{  

public:  
   ServerGame(void);  
   ~ServerGame(void);  

   void update();  
   bool receiveFromClients();  
   void sendGameStatePackets(); // Ensure this declaration matches the definition in ServerGame.cpp  
   void writeToGameState(); // Ensure this declaration matches the definition in ServerGame.cpp

   

private:  
   static unsigned int client_id;  
   ServerNetwork* network;  
   char network_data[MAX_PACKET_SIZE];  

   GameStatePacket GameState;
   PlayerIntentPacket PlayerIntent;  
   PhysicsSystem physicsSystem; // Add this line to include the physics system

   PlayerBehaviorComponent* playerBehaviors[4] = { nullptr, nullptr, nullptr, nullptr }; // Array to hold player behavior components] = { nullptr, nullptr, nullptr, nullptr }; // Array of player behaviors
   
};

//extern GameStatePacket GameState;