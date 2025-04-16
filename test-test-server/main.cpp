// main.cpp : Defines the entry point for the console application.
//

// may need #include "stdafx.h" in visual studio
#include "stdafx.h"
#include "ServerGame.h"
// used for multi-threading
#include <process.h>

void serverLoop();

ServerGame * server;

int main()
{

	// initialize the server
	server = new ServerGame();
    serverLoop();
}

void serverLoop() 
{ 
    while(true) 
    {
        server->update();
    }
}

