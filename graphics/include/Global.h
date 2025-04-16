#pragma once

struct PlayerStats {
    unsigned int ID : 4; //4 bits
    int maxHP = 250;
    int currHP = 250; //damage from powerups will be whole numbers
};