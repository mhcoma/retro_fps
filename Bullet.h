#pragma once

#include <algorithm>

#include "Actor.h"
#include "Manager.h"
#include "Player.h"

#include "glm.hpp"
#include "ext/scalar_constants.hpp"

class Bullet : public Actor {
public:
	Bullet();
	~Bullet();

	float angle = 1.0;
	double speed = 32.0;

	bool is_enemys_bullet = false;

	void update();
	void draw();

	bool collision_block(int block, bool under_ground, bool enemy_block);
};
