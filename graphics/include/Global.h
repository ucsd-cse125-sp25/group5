#pragma once

enum class ElementType {
    WATER,
    FIRE,
    EARTH,
    WOOD,
    METAL
};

enum class GameState {
    LOBBY,
    MATCH
};

struct OtherPlayerStats {
    //damage from powerups will be whole numbers
    int maxHP = 250, currHP = 250;
    int maxWater = 100, currWater = 50;
    int maxFire = 100, currFire = 50;
    int maxEarth = 100, currEarth = 50;
    int maxWood = 100, currWood = 50;
    int maxMetal = 100, currMetal = 50;
    ElementType currElem = ElementType::WATER;
    //unsigned int ID : 4;
};

constexpr float MANA_RADIUS = 75.0f;
constexpr float SPIN_DURATION = 0.5f;