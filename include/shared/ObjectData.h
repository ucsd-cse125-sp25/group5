#pragma once
#include <string.h> 
#include "core.h"


#define MAX_HP 120;
#define MAX_MANA 100;


static glm::vec3 island_extents = glm::vec3(10.0f, 1.0f, 10.0f);
static glm::vec3 woodProjExtents = glm::vec3(0.1f, 0.1f, 0.1f);
static glm::vec3 fireProjExtents = glm::vec3(0.5f, 0.5f, 0.5f);

static float woodProjSpeed = 25.0f;
static float fireProjSpeed = 12.0f;

static int projDamage[5] = { 10, 20, 30, 40, 50 };

enum EntityType {
	ENTITY,
	PLAYER,
	CUBE,
	ISLAND,
	FLAG,
	METAL_PROJ,
	WOOD_PROJ,
	WATER_PROJ,
	FIRE_PROJ,
	EARTH_PROJ,
	COLLIDER
};

enum PowerType {
	METAL,
	WOOD,
	WATER,
	FIRE,
	EARTH
};

enum KillfeedType {
	KILL,
	FLAGSTEAL,
	FLAGDROP,
	FLAGPICKUP
};

struct Entity {
	unsigned int id;
	EntityType type;
	glm::mat4 model;
	glm::vec3 ext;
};

struct PlayerStats {
	unsigned int hp = MAX_HP;
	bool alive = true;
	bool hasFlag = false;
	bool moving = false;
	bool inAir = false;
	unsigned int mana[5] = {100, 100, 100, 100, 100};
	PowerType activePower = METAL;
};

struct KillfeedItem {
	int victim;
	int attacker;
	KillfeedType type;
	float lifetime;
};