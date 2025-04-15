#pragma once  
#include "ServerNetwork.h"  
#include "NetworkData.h"  

class ServerGame  
{  
public:  
   ServerGame(void);  
   ~ServerGame(void);  

   void update();  
   void receiveFromClients();  
   void sendActionPackets(); // Ensure this declaration matches the definition in ServerGame.cpp  

private:  
   static unsigned int client_id;  
   ServerNetwork* network;  
   char network_data[MAX_PACKET_SIZE];  
   PlayerIntentPacket PlayerIntent;  
   GameStatePacket GameState;  
};