#pragma once
#include <string.h> 
#include "core.h"


#define MAX_HP 100;
#define MAX_MANA 100;

enum EntityType {
	ENTITY,
	PLAYER,
	CUBE
};

enum PowerType {
	METAL,
	WOOD,
	WATER,
	FIRE,
	EARTH
};

struct Entity {
	unsigned int id;
	EntityType type;
	glm::mat4 model;
};

struct PlayerStats {
	unsigned int hp = MAX_HP;
	bool alive = true;
	bool hasFlag = false;
	unsigned int mana[5] = {100, 100, 100, 100, 100};
	PowerType activePower = METAL;
};