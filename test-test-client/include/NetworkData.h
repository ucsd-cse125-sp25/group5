#pragma once
#include <string.h>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

    INIT_CONNECTION = 0,

    ACTION_EVENT = 1,

    FORWARD = 2,
    BACKWARD = 3,
    LEFT = 4,
    RIGHT = 5,

    POSITION = 6


};

struct Packet {

    UINT8 packet_type;

    void serialize(char * data) {
        memcpy(data, this, sizeof(Packet));
    }

    void deserialize(char * data) {
        memcpy(this, data, sizeof(Packet));
    }
};

struct PositionPacket: Packet {

    float x;
    float y;
    float z;

    void serialize(char* data) {
        memcpy(data, this, sizeof(PositionPacket));
    }

    void deserialize(char* data) {
        memcpy(this, data, sizeof(PositionPacket));
    }
};