#pragma once
#include "../../group5/include/shared/ObjectData.h"
//#include "Scene.h"
#include "network/ClientGame.h"

enum class ElementType {
    METAL,
    WOOD,
    WATER,
    FIRE,
    EARTH
};

struct UIData {
    //damage from powerups will be whole numbers
    int maxHP = 120, currHP = 120;
    int maxWater = 100, currWater = 50;
    int maxFire = 100, currFire = 50;
    int maxEarth = 100, currEarth = 50;
    int maxWood = 100, currWood = 50;
    int maxMetal = 100, currMetal = 50;

    ElementType currElem = ElementType::WATER;
    float seconds = 10;
    KillfeedItem killfeed[KILLFEED_LENGTH];
    //unsigned int ID : 4;
};

extern int WINDOWWIDTH;
extern int WINDOWHEIGHT;