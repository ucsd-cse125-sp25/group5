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

static float woodProjSpeed = 40.0f;
static float fireProjSpeed = 12.0f;
static float waterProjSpeed = 50.0f;

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
	int hp = MAX_HP;
	int maxHP = MAX_HP;
	bool alive = true;
	bool hasFlag = false;
	bool moving = false;
	bool inAir = false;
	bool underwater = false;
	bool low_oxygen = false; 

	glm::vec3 grappleTarget = glm::vec3(0.0f, 0.0f, 0.0f);

	int mana[5] = { 100, 100, 100, 100, 100 };
	int prevMana[5] = { 100, 100, 100, 100, 100 };
	PowerType activePower = METAL;

	int movementPowerupFlag[5] = { 0, 0, 0, 0, 0};
	int attackPowerupFlag[5] = { 0, 0, 0, 0, 0 };
	bool damageFlag = false; // used to indicate if the player has taken damage this tick
	bool dealtDamageFlag = false; // used to indicate if the player has dealt damage this tick
	int closestPlayer = -1; // used to indicate the closest player to this player
	float waveAudioFlag = 0.0f; // used to indicate if the player is underwater and the wave audio should be played
	float windAudioFlag = 0.0f; // used to indicate if the player is in the air and the wind audio should be played	
	bool hpPickupFlag = false; // used to indicate if the player has picked up an HP pickup this tick
	bool manaPickupFlag = false; // used to indicate if the player has picked up a Mana pickup this tick
};

struct KillfeedItem {
	int victim;
	int attacker;
	KillfeedType type;
	float lifetime;
};


const static std::vector<std::tuple<std::string, glm::vec3, std::string>> mapObjects = {
	std::make_tuple("lantern_1", glm::vec3(15.59998f, 111.16f, -6.83997f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-10.70001f, 110.32f, -7.06006f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-13.10001f, 43.19001f, -28.04004f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-26.20001f, 117.16f, 22.80005f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(21.f, 113.76f, -22.79004f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(21.f, 51.86f, -22.79004f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(17.21997f, 48.03f, -7.65002f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-17.01999f, 41.26f, 29.40002f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-42.02002f, 36.68001f, 21.5f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(31.79999f, 53.26f, 3.59998f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(0.f, 33.56f, 13.90002f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-19.10001f, 96.31f, 10.40002f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-35.79999f, 100.28f, -0.34998f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-26.20001f, 45.86f, 22.80005f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-42.02002f, 99.81f, 23.59998f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-17.01999f, 104.39f, 27.5f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-13.10001f, 102.56f, -25.09998f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(0.f, 96.68999f, 13.30005f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(31.79999f, 116.39f, 3.59998f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(41.29999f, 105.16f, -12.34998f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-2.70001f, 47.19001f, -7.06006f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-38.70001f, 37.15f, -0.34998f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(38.79999f, 49.76f, -12.34998f), ".obj"),
std::make_tuple("lantern_1", glm::vec3(-19.10001f, 33.18001f, 14.80005f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(1.96653f, 3.81236f, -2.8117f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-2.28682f, 95.16336f, -10.48935f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(6.82826f, 3.54739f, -9.805f), ".obj"),
std::make_tuple("Island Fragment 9", glm::vec3(-15.89999f, 45.f, 42.59998f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(-11.32812f, 2.96116f, 9.13986f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(-46.58412f, 91.3877f, 14.96076f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(7.24136f, 3.57808f, -9.69731f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(11.03601f, 3.15667f, 7.60592f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-6.00394f, 89.89769f, -3.45726f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(-4.49011f, 95.74604f, -6.79244f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(-11.84024f, 2.98885f, 9.44107f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(0.f, 0.f, -40.f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(32.16061f, 140.298f, 47.08108f), ".obj"),
std::make_tuple("Island Fragment 7", glm::vec3(11.79999f, 50.3f, -44.43994f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(-30.62782f, 28.9101f, -26.06896f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(36.68578f, 139.9776f, 40.95185f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(12.41852f, 50.11553f, -37.17772f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(28.65539f, 50.29774f, 29.91596f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(8.f, 88.2f, 38.70001f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-4.21041f, 92.31052f, 1.2307f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(23.55316f, 138.9278f, -44.77491f), ".obj"),
std::make_tuple("Bush 1", glm::vec3(-2.69594f, 82.17828f, -10.90516f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(25.45121f, 139.877f, 30.20447f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(-11.31971f, 2.50787f, -12.1859f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(38.00745f, 139.5906f, 36.55281f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(-10.98471f, 2.50538f, -12.125f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-39.76403f, 3.21849f, -1.22659f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(25.3499f, 49.93172f, 35.75553f), ".obj"),
std::make_tuple("Cloud 5", glm::vec3(-54.10001f, 179.2f, 33.29999f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(0.11692f, 3.87086f, -2.81695f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(30.87567f, 139.0201f, -54.63417f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(-0.23399f, 3.81667f, -3.26408f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-4.1002f, 91.94216f, 3.08758f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-2.72014f, 26.37354f, 4.46737f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(11.49682f, 3.12803f, 7.48781f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-8.62766f, 91.11446f, -1.37759f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-6.09545f, 89.27408f, -4.04492f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(38.90921f, 139.5677f, 36.89639f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(7.02179f, 3.58705f, -9.57208f), ".obj"),
std::make_tuple("Island Fragment 3", glm::vec3(32.f, 62.09999f, -18.69995f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-13.78455f, 125.3935f, 6.56173f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(20.05908f, 49.94776f, 37.94172f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(-2.97024f, 3.73885f, 5.61108f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(27.92787f, 140.1129f, 37.4002f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(40.f, 0.f, 0.f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(38.82149f, 139.497f, 54.97661f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(-27.92001f, 96.09999f, -48.5f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(0.94732f, 3.77813f, -3.49494f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(11.37179f, 3.1525f, 7.70335f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(39.02842f, 3.20225f, -0.75159f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(4.1722f, 3.7425f, 8.15257f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(0.20133f, 3.87128f, -2.96556f), ".obj"),
std::make_tuple("Island Fragment 9", glm::vec3(-34.35999f, 135.17f, -51.62f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(4.12496f, 33.2476f, -26.10373f), ".obj"),
std::make_tuple("HealthRing", glm::vec3(27.90002f, 137.4f, -53.09998f), ".obj"),
std::make_tuple("ManaRing", glm::vec3(27.90002f, 139.42f, -53.09998f), ".obj"),
std::make_tuple("Island Fragment 6", glm::vec3(27.90002f, 137.4f, -53.09998f), ".obj"),
std::make_tuple("Bush 2", glm::vec3(-4.90483f, 96.51006f, 7.36961f), ".obj"),
std::make_tuple("Ivy 1", glm::vec3(-6.71537f, 77.00404f, 9.30465f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-1.70561f, 95.06258f, -10.53425f), ".obj"),
std::make_tuple("Cloud 2", glm::vec3(67.24091f, 178.5f, 32.59998f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(35.0714f, 139.6646f, -63.06315f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(34.61203f, 139.6415f, 58.75332f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-1.65816f, 3.16225f, -39.79654f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(14.34861f, 31.98507f, -8.63242f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(28.60903f, 138.789f, -60.63492f), ".obj"),
std::make_tuple("Island Fragment 3", glm::vec3(-27.39999f, 27.10001f, -24.35999f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(28.2129f, 50.29991f, 29.74106f), ".obj"),
std::make_tuple("Island 01", glm::vec3(27.09998f, 136.4f, 44.59998f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(-6.09998f, 35.2f, -14.17004f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(2.05754f, 3.76488f, -3.36891f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(11.02809f, 55.02708f, -60.31599f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(6.09998f, 90.5f, -34.09998f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(6.49327f, 3.54987f, -9.8659f), ".obj"),
std::make_tuple("Cloud 4", glm::vec3(10.90002f, 175.2f, 49.90002f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(30.66019f, 138.9865f, -57.62749f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(20.60293f, 138.7955f, -62.41269f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-6.31983f, 86.78968f, -7.5833f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(37.21108f, 139.3919f, -48.15234f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(1.7453f, 3.54787f, -12.1859f), ".obj"),
std::make_tuple("Ivy 1", glm::vec3(-2.6556f, 49.32853f, -7.54841f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(-11.71521f, 2.96438f, 9.22554f), ".obj"),
std::make_tuple("Building02", glm::vec3(28.29308f, 144.4688f, 44.20121f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(38.30642f, 139.5544f, 36.4839f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-37.23065f, 124.5776f, 6.4222f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(27.99369f, 140.1457f, 37.87731f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(38.40118f, 63.44089f, -14.54749f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-7.37733f, 1.63125f, 21.62258f), ".obj"),
std::make_tuple("Island 02", glm::vec3(0.f, 0.f, 0.f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(-2.29999f, 23.16f, 2.45996f), ".obj"),
std::make_tuple("Island Fragment 9", glm::vec3(16.56f, 105.7f, 19.80005f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(17.75753f, 49.7557f, 26.97935f), ".obj"),
std::make_tuple("HealthRing", glm::vec3(22.35846f, 48.3f, 27.86133f), ".obj"),
std::make_tuple("ManaRing", glm::vec3(22.35846f, 50.32f, 27.86133f), ".obj"),
std::make_tuple("Island Fragment 6", glm::vec3(22.35846f, 48.3f, 27.86133f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(21.56752f, 138.7841f, -57.81484f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(2.62582f, 33.22628f, -27.54451f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(-10.99471f, 2.95448f, 9.11278f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(38.17863f, 139.6191f, 36.81228f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(15.17693f, 52.96064f, -53.46376f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-13.05777f, 24.98194f, -9.71237f), ".obj"),
std::make_tuple("Cloud 3", glm::vec3(27.70001f, 59.f, 5.93628f), ".obj"),
std::make_tuple("Platform 07", glm::vec3(23.56f, 64.57751f, -13.59f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(12.21732f, 3.11813f, 7.37506f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(-11.77077f, 2.5505f, -11.78744f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(30.98923f, 139.0092f, -58.12441f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(-12.20001f, 21.8f, -11.64807f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(4.93551f, 3.69123f, 7.54109f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(-3.30078f, 3.7425f, 5.77557f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(4.62327f, 3.69987f, 7.7541f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(10.97932f, 139.1434f, 35.95354f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(46.04911f, 138.7486f, 52.15293f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-36.46031f, 28.65662f, -16.31469f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(1.62476f, 3.58685f, -11.95193f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(1.29423f, 3.5905f, -11.78744f), ".obj"),
std::make_tuple("Cloud 1", glm::vec3(48.90002f, 184.4f, -56.59998f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(-33.16976f, 28.95654f, -24.96454f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(11.77327f, 3.54987f, -1.0459f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(11.3222f, 3.5925f, -0.64744f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(25.8972f, 49.8856f, 35.98935f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(11.38692f, 3.21086f, 8.05305f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(0.22682f, 3.78803f, -3.38219f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(1.7453f, 3.77352f, -3.1559f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-10.63756f, 89.5765f, 1.50457f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(-12.17602f, 2.99302f, 9.34365f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(-12.17077f, 2.9925f, 9.60556f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(-3.30602f, 3.74302f, 5.51365f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(6.7145f, 3.58872f, -9.5217f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(26.14828f, 140.088f, 37.57539f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(2.08029f, 3.77104f, -3.095f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(-2.12471f, 3.70448f, 5.28278f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(11.65273f, 3.58885f, -0.81192f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(15.76206f, 3.25354f, 1.46213f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(6.57218f, 2.4055f, 21.2857f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(2.49339f, 3.80173f, -2.98731f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(0.61391f, 3.78481f, -3.46787f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(3.94f, 30.05f, -27.5f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(16.84529f, 139.9683f, 44.7097f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(-40.f, 0.f, 0.f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(-45.95999f, 88.2f, 13.5f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-0.07435f, 3.21264f, 39.05658f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(4.95826f, 3.69738f, 7.815f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(30.10292f, 138.8969f, -58.04607f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(-2.84521f, 3.71438f, 5.39554f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-38.15992f, 124.7162f, 6.45974f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-12.21616f, 25.02345f, -9.67136f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(15.15002f, 28.8f, -9.92004f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(-3.96746f, 95.7592f, -6.21864f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(28.22479f, 140.1085f, 37.57925f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(1.198f, 3.21845f, -39.73864f), ".obj"),
std::make_tuple("Platform 02", glm::vec3(17.51009f, 108.119f, 17.29786f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(39.74783f, 3.21074f, -0.96869f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(11.88391f, 3.12481f, 7.40213f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-0.03184f, 3.71129f, 9.96645f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-5.97084f, 38.39248f, -13.62316f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(30.235f, 140.0824f, 39.0961f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(0.f, 0.f, 40.f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-3.25464f, 92.32124f, -3.3444f), ".obj"),
std::make_tuple("Ivy 1", glm::vec3(-19.32986f, 98.38475f, 10.88722f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(6.80551f, 3.54123f, -10.07891f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(-11.44024f, 2.54685f, -11.95193f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(4.16358f, 3.01423f, 17.71379f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(11.92893f, 50.03971f, -37.51847f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-38.98072f, 3.22711f, -1.14263f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(-11.77602f, 2.55102f, -12.04935f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(-46.33546f, 91.38461f, 14.82605f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(-2.8707f, 3.79763f, 5.81216f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(11.35423f, 139.7189f, 42.35329f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-26.78171f, 124.9984f, 0.06736f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(31.77402f, 139.0901f, -58.71281f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-0.64956f, 72.07571f, -10.25465f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(-4.51188f, 95.7588f, -6.02858f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-3.13802f, 26.36352f, 1.52971f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-37.74761f, 124.7947f, 5.87635f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(4.16695f, 3.74302f, 7.89065f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(-11.7407f, 3.04763f, 9.64216f), ".obj"),
std::make_tuple("HealthRing", glm::vec3(1.59998f, 82.59999f, -57.19995f), ".obj"),
std::make_tuple("ManaRing", glm::vec3(1.59998f, 84.61999f, -57.19995f), ".obj"),
std::make_tuple("Island Fragment 6", glm::vec3(1.59998f, 82.59999f, -57.19995f), ".obj"),
std::make_tuple("Island Fragment 3", glm::vec3(-47.5f, 108.3f, -24.35999f), ".obj"),
std::make_tuple("Cloud 6", glm::vec3(-36.5f, 192.6f, -36.59998f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(11.47133f, 3.21128f, 7.90444f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(12.02603f, 51.54028f, -48.38845f), ".obj"),
std::make_tuple("Ivy 1", glm::vec3(-8.36f, 94.26015f, 10.839f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(43.46362f, 138.8872f, 57.28681f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(12.08551f, 3.54123f, -1.25891f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(20.78007f, 49.84623f, 33.74038f), ".obj"),
std::make_tuple("Cloud 7", glm::vec3(-16.60001f, 177.7f, -65.19995f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(19.67197f, 49.9271f, 37.87117f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(24.25676f, 140.0804f, 37.51407f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(37.11164f, 63.5581f, -14.72739f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-19.84257f, 125.5961f, 3.58737f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(24.26521f, 140.0767f, 37.25352f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(1.28898f, 3.59102f, -12.04935f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(38.64526f, 139.5283f, 36.50755f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(42.53753f, 138.7204f, 59.8961f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(11.04125f, 3.15615f, 7.86784f), ".obj"),
std::make_tuple("HealthRing", glm::vec3(-51.70001f, 60.2f, 0.f), ".obj"),
std::make_tuple("ManaRing", glm::vec3(-51.70001f, 62.22f, 0.f), ".obj"),
std::make_tuple("Island Fragment 6", glm::vec3(-51.70001f, 60.2f, 0.f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-44.3383f, 91.4223f, 13.91206f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-5.10563f, 95.82728f, 8.78711f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(-2.45812f, 3.71116f, 5.30986f), ".obj"),
std::make_tuple("Cloud 2", glm::vec3(-34.39999f, 108.4f, 27.29999f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(19.88471f, 138.4034f, -62.4052f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(38.90789f, 140.0034f, 48.04195f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(2.05754f, 3.53923f, -12.39891f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(14.11882f, 2.65919f, -9.72108f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(3.71167f, 33.23795f, -26.07866f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(-2.95511f, 3.79721f, 5.96078f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(30.47071f, 138.9695f, -58.88537f), ".obj"),
std::make_tuple("Island Fragment 5", glm::vec3(-9.10001f, 94.09999f, -10.19995f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(38.34382f, 139.6511f, 37.22882f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(2.08029f, 3.54538f, -12.125f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(-11.00746f, 2.49923f, -12.39891f), ".obj"),
std::make_tuple("Landbridge 1", glm::vec3(-36.39999f, 124.f, 10.f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(4.50273f, 3.73885f, 7.98808f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(0.10179f, 3.8125f, -3.16665f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(41.41276f, 3.20963f, 1.24092f), ".obj"),
std::make_tuple("Platform 08", glm::vec3(33.58039f, 64.19033f, -17.12632f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-14.91045f, 1.49548f, 15.42009f), ".obj"),
std::make_tuple("Small Plant 4", glm::vec3(-11.82511f, 3.04721f, 9.79078f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(17.37464f, 139.8667f, 38.10801f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-0.71309f, 3.19719f, 39.65949f), ".obj"),
std::make_tuple("Small Plant 1", glm::vec3(-0.22875f, 3.81615f, -3.00216f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(38.76629f, 139.4509f, 36.06491f), ".obj"),
std::make_tuple("Ivy 2", glm::vec3(-8.10578f, 78.08849f, 10.21605f), ".obj"),
std::make_tuple("Cloud 9", glm::vec3(-19.70001f, 172.4f, -2.80005f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(2.27382f, 3.8107f, -2.86208f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-3.02429f, 90.76975f, -6.3118f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(12.10826f, 3.54739f, -0.985f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(28.17524f, 140.0772f, 37.18951f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(-33.92185f, 28.98293f, -23.19962f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-10.93763f, 3.54036f, 1.37599f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(26.39674f, 140.0972f, 37.77151f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-6.8964f, 88.35799f, 2.81847f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-35.09138f, 28.49525f, -15.60676f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(37.99756f, 63.62913f, -15.44553f), ".obj"),
std::make_tuple("Hanging Ivy 1", glm::vec3(-3.89592f, 91.89468f, 3.65056f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(33.17355f, 140.188f, 42.23137f), ".obj"),
std::make_tuple("Small Plant 2", glm::vec3(38.1382f, 139.4888f, 35.91212f), ".obj"),
std::make_tuple("Small Island Fragment 1", glm::vec3(0.f, 0.f, 0.f), ".obj"),
std::make_tuple("Airplane Plant", glm::vec3(-4.045f, 95.63338f, 7.79734f), ".obj"),
std::make_tuple("Small Plant 3", glm::vec3(11.31695f, 3.59302f, -0.90935f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(0.f, 3.56f, 0.f), ".obj"),
//std::make_tuple("h2", glm::vec3(-42.99f, 90.488f, 13.42f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(-20.38999f, 127.16f, 10.f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(-12.20001f, 25.8f, -11.64807f), ".obj"),
//std::make_tuple("HealthRing", glm::vec3(-20.38999f, 127.16f, 10.f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(27.09998f, 140.76f, 44.59998f), ".obj"),
//std::make_tuple("HealthRing", glm::vec3(27.09998f, 140.45f, 44.59998f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(-2.29999f, 27.16f, 2.45996f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(3.94f, 34.05f, -27.5f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(6.09998f, 94.5f, -34.09998f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(-45.95999f, 92.2f, 13.5f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(8.f, 92.2f, 38.70001f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(15.15002f, 32.8f, -9.92004f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(-6.09998f, 35.2f, -14.17004f), ".obj"),
//std::make_tuple("h1", glm::vec3(38.78f, 65.2f, -15.52f), ".obj"),
//std::make_tuple("ManaRing", glm::vec3(-27.92001f, 100.1f, -48.5f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(0.f, 35.f, 13.90002f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-19.10001f, 97.75f, 10.40002f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(31.79999f, 54.7f, 3.59998f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-42.02002f, 38.12001f, 21.5f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-17.01999f, 42.7f, 29.40002f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(17.21997f, 49.47f, -7.65002f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-38.70001f, 38.59f, -0.34998f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-19.10001f, 34.62001f, 14.80005f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(38.79999f, 51.2f, -12.34998f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-2.70001f, 48.63f, -7.06006f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(31.79999f, 117.83f, 3.59998f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(41.29999f, 106.6f, -12.34998f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(0.f, 98.12999f, 13.30005f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-13.10001f, 104.f, -25.09998f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-26.20001f, 47.3f, 22.80005f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-42.02002f, 101.25f, 23.59998f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-17.01999f, 105.83f, 27.5f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-35.79999f, 101.72f, -0.34998f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(15.59998f, 112.6f, -6.83997f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-10.70001f, 111.76f, -7.06006f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(21.f, 53.3f, -22.79004f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(21.f, 115.2f, -22.79004f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-26.20001f, 118.6f, 22.80005f), ".obj"),
std::make_tuple("Large Crate", glm::vec3(-13.10001f, 44.63f, -28.04004f), ".obj"),




};