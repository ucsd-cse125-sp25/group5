#pragma once

struct PlayerStats {
    unsigned int ID : 4; //4 bits
    int maxHP = 250;
    int currHP = 250; //damage from powerups will be whole numbers
    int maxWater = 100;
    int currWater = 50;
    int maxFire = 100;
    int currFire = 75;
    int maxEarth = 100;
    int currEarth = 15;
    int maxWood = 100;
    int currWood = 25;
    int maxMetal = 100;
    int currMetal = 95;
};

enum class GameState {
    LOBBY,
    MATCH
};