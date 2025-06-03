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
	std::make_tuple("Island 01", glm::vec3(27.09998f, 136.4f, 44.59998f), ".obj"),
	std::make_tuple("Landbridge 1", glm::vec3(-36.39999f, 124.f, 10.f), ".obj"),
	std::make_tuple("Cloud 3", glm::vec3(27.70001f, 59.f, 5.936279f), ".obj"),
	std::make_tuple("Cloud 6", glm::vec3(-36.5f, 192.6f, -36.59998f), ".obj"),
	std::make_tuple("Cloud 9", glm::vec3(-19.70001f, 172.4f, -2.800049f), ".obj"),
	std::make_tuple("Cloud 7", glm::vec3(-16.60001f, 177.7f, -65.19995f), ".obj"),
	std::make_tuple("Cloud 2", glm::vec3(67.24091f, 178.5f, 32.59998f), ".obj"),
	std::make_tuple("Cloud 5", glm::vec3(-54.10001f, 179.2f, 33.29999f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(-12.20001f, 21.8f, -11.64807f), ".obj"),
	std::make_tuple("Island Fragment 6", glm::vec3(22.35846f, 48.3f, 27.86133f), ".obj"),
	std::make_tuple("Island Fragment 3", glm::vec3(-27.39999f, 27.10001f, -24.35999f), ".obj"),
	std::make_tuple("Cloud 1", glm::vec3(48.90002f, 184.4f, -56.59998f), ".obj"),
	std::make_tuple("Cloud 4", glm::vec3(10.90002f, 175.2f, 49.90002f), ".obj"),
	std::make_tuple("Island 02", glm::vec3(0.f, 0.f, 0.f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(-40.f, 0.f, 0.f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(35.09998f, 2.300003f, 0.f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(0.0f, 0.0f, 40.0f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(0.0f, 0.f, -40.0f), ".obj"),
	std::make_tuple("Island Fragment 5", glm::vec3(-9.100006f, 94.09999f, -10.19995f), ".obj"),
	std::make_tuple("Island Fragment 6", glm::vec3(-51.70001f, 60.2f, 0.0f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(0.0f, 0.0f, 0.0f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(-2.299988f, 23.16f, 2.459961f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(15.15002f, 28.8f, -9.920044f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(3.940002f, 30.05f, -27.5f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(0.0f, 35.0f, 13.90002f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-38.70001f, 38.59f, -0.3499756f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-13.10001f, 44.63f, -28.04004f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-2.700012f, 48.63f, -7.060059f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(38.79999f, 51.2f, -12.34998f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(31.79999f, 54.7f, 3.599976f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(21.f, 53.3f, -22.79004f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(17.21997f, 49.47f, -7.650024f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-26.20001f, 47.3f, 22.80005f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-17.01999f, 42.7f, 29.40002f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-42.02002f, 38.12001f, 21.5f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-19.10001f, 34.62001f, 14.80005f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(-6.099976f, 35.2f, -14.17004f), ".obj"),
	std::make_tuple("Island Fragment 7", glm::vec3(11.79999f, 50.3f, -44.43994f), ".obj"),
	std::make_tuple("Island Fragment 9", glm::vec3(-15.89999f, 45.f, 42.59998f), ".obj"),
	std::make_tuple("Island Fragment 6", glm::vec3(1.599976f, 82.59999f, -57.19995f), ".obj"),
	std::make_tuple("Island Fragment 9", glm::vec3(16.56f, 105.7f, 19.80005f), ".obj"),
	std::make_tuple("Island Fragment 3", glm::vec3(-47.5f, 108.3f, -24.35999f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(0.f, 98.12999f, 13.30005f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-35.79999f, 101.72f, -0.3499756f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-13.10001f, 104.f, -25.09998f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-10.70001f, 111.76f, -7.060059f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(15.59998f, 112.6f, -6.839966f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(21.f, 115.2f, -22.79004f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(31.79999f, 117.83f, 3.599976f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(41.29999f, 106.6f, -12.34998f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-19.10001f, 97.75f, 10.40002f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-42.02002f, 101.25f, 23.59998f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-17.01999f, 105.83f, 27.5f), ".obj"),
	std::make_tuple("Large Crate", glm::vec3(-26.20001f, 118.6f, 22.80005f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(-27.92001f, 96.09999f, -48.5f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(6.099976f, 90.5f, -34.09998f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(-45.95999f, 88.2f, 13.5f), ".obj"),
	std::make_tuple("Small Island Fragment 1", glm::vec3(8.f, 88.2f, 38.70001f), ".obj"),
	std::make_tuple("Cloud 2", glm::vec3(-34.39999f, 108.4f, 27.29999f), ".obj"),
	std::make_tuple("Island Fragment 3", glm::vec3(32.f, 62.09999f, -18.69995f), ".obj"),
	std::make_tuple("Island Fragment 6", glm::vec3(27.90002f, 137.4f, -53.09998f), ".obj"),
	std::make_tuple("Island Fragment 9", glm::vec3(-34.35999f, 135.17f, -51.62f), ".obj"),
};