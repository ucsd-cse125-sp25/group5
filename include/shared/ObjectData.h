#pragma once
#include <string.h> 
#include "core.h"


#define MAX_HP 100;
#define MAX_MANA 100;


static glm::vec3 island_extents = glm::vec3(10.0f, 1.0f, 10.0f);
static glm::vec3 woodProjExtents = glm::vec3(0.1f, 0.1f, 0.1f);

static float woodProjSpeed = 25.0f;

enum EntityType {
	ENTITY,
	PLAYER,
	CUBE,
	ISLAND,
	D_CUBE,
	METAL_PROJ,
	WOOD_PROJ,
	WATER_PROJ,
	FIRE_PROJ,
	EARTH_PROJ
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