#pragma once

enum class ElementType {
    METAL,
    WOOD,
    WATER,
    FIRE,
    EARTH
};

enum class GameState {
    LOBBY,
    MATCH
};

struct UIData {
    //damage from powerups will be whole numbers
    int maxHP = 100, currHP = 100;
    int maxWater = 100, currWater = 50;
    int maxFire = 100, currFire = 50;
    int maxEarth = 100, currEarth = 50;
    int maxWood = 100, currWood = 50;
    int maxMetal = 100, currMetal = 50;
    ElementType currElem = ElementType::METAL;
    //unsigned int ID : 4;
};

extern int WINDOWWIDTH;
extern int WINDOWHEIGHT;