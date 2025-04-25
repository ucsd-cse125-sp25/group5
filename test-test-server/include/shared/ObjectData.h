#pragma once
#include <string.h> 
#include "core.h"

enum EntityType {
	ENTITY,
	PLAYER
};

struct Entity {

	unsigned int id;
	EntityType type;
	glm::mat4 model;
};