// main.cpp : Defines the entry point for the console application.
//

// may need #include "stdafx.h" in visual studio
#include "stdafx.h"
#include "ClientGame.h"

void clientLoop(void);

ClientGame * client;

int main()
{

    // initialize the client 
    client = new ClientGame();

	clientLoop();
}

void clientLoop()
{
    while(true)
    {
        //do game stuff
        client->update();
    }
}
