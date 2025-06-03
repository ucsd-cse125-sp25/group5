#ifndef PLAYER_H
#define PLAYER_H
#include "Skeleton.h"
#include "Animation.h"
#include <chrono>

class Player {

public:
	Skeleton* skeleton;
	Animation* animation;
	int type; //which object
	int anim; //which animation clip
	int mode; //which segment of the clip

	std::chrono::steady_clock::time_point realStartTime;

	Player(Skeleton* skeleton, Animation* animation, std::chrono::steady_clock::time_point realStartTime);

	void update();
	void Jump();
	void Land();
};


#endif // PLAYER_H