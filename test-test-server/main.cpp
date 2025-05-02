// main.cpp : Defines the entry point for the console application.
//

// may need #include "stdafx.h" in visual studio
#include "stdafx.h"
#include "ServerGame.h"
// used for multi-threading
#include <process.h>
#include <iostream>
pair<vec3, float> SATOverlapTestExperimental(AABB a, AABB b);

void serverLoop();

ServerGame * server;

int main()
{

	// initialize the server
    AABB a;
    a.min = vec3(0, 0, 0);
    a.max = vec3(4, 4, 4);

    AABB b;
    b.min = vec3(1, 2, 2);
    b.max = vec3(4, 4, 6);

    std::pair<vec3, float> ret = SATOverlapTestExperimental(a, b);

    cout << ret.first.x << " " << ret.first.y << " " << ret.first.z << " " << ret.second << endl;

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

