#pragma once
#include <string.h> 
#include "core.h"
#include <utility>
#include <string> // Include for std::string
#include <vector>
#include <glm/vec3.hpp> // Ensure glm::vec3 is included
#include <tuple>

#define MAX_HP 120;
#define MAX_MANA 100;


static glm::vec3 island_extents = glm::vec3(10.0f, 1.0f, 10.0f);
static glm::vec3 woodProjExtents = glm::vec3(0.1f, 0.1f, 0.1f);
static glm::vec3 fireProjExtents = glm::vec3(0.5f, 0.5f, 0.5f);
static glm::vec3 groundExtents = glm::vec3(1000.0f, 1.0f, 1000.0f);

static float woodProjSpeed = 25.0f;
static float fireProjSpeed = 12.0f;

static int particleTimeLimit = 20; // 20 ticks

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
	COLLIDER,
	BOUNDS,
	HP_PICKUP,
	MANA_PICKUP
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

enum GamePhase {
	WAITING,
	PRE_GAME,
	IN_GAME,
	POST_GAME
};

enum MoonPhase {
	NEW_MOON,
	FIRST_QUARTER,
	FULL_MOON
};

struct Entity {
	unsigned int id;
	EntityType type;
	glm::mat4 model;
	glm::vec3 ext;
};

struct PlayerStats {
	unsigned int hp = MAX_HP;
	unsigned int maxHP = MAX_HP;
	bool alive = true;
	bool hasFlag = false;
	bool moving = false;
	bool inAir = false;
	bool underwater = false;
	
	glm::vec3 grappleTarget = glm::vec3(0.0f, 0.0f, 0.0f);

	unsigned int mana[5] = { 100, 100, 100, 100, 100 };
	PowerType activePower = METAL;

	int movementPowerupFlag[5] = { 0, 0, 0, 0, 0};
	int attackPowerupFlag[5] = { 0, 0, 0, 0, 0 };
	bool damageFlag = false; // used to indicate if the player has taken damage this tick
	bool dealtDamageFlag = false; // used to indicate if the player has dealt damage this tick
};

struct KillfeedItem {
	int victim;
	int attacker;
	KillfeedType type;
	float lifetime;
};


const static std::vector<std::tuple<std::string, glm::vec3, std::string>> mapObjects = {
    std::make_tuple("floating-island", glm::vec3(0.0f, 0.0f, 0.0f), ".obj"),
    std::make_tuple("corridor1", glm::vec3(0.0f, 0.0f, 10.0f), ".obj"),
    std::make_tuple("corridor-2", glm::vec3(10.0f, 0.0f, 0.0f), ".obj"),
    std::make_tuple("floating-island", glm::vec3(10.0f, 20.0f, 0.0f), ".obj"),
    std::make_tuple("island", glm::vec3(30.0f, 10.0f, 0.0f), ".obj"),
	/*std::make_tuple("Platform 1", glm::vec3(0.0f, 100.0f, 00.0f), ".fbx"),
	std::make_tuple("Island Fragment 8", glm::vec3(30.0f, 20.0f, 0.0f), ".fbx"),
	std::make_tuple("Island Fragment 5", glm::vec3(0.0f, 50.0f, 30.0f), ".fbx"),*/
	std::make_tuple("Island Fragment 3", glm::vec3(10.0f, 40.0f, 0.0f), ".obj"),
};