#pragma once

#include "Actor.h"
#include "CamActor.h"

#include "Manager.h"
#include "Player.h"

class Enemy : public CamActor {
public:
	Enemy();
	~Enemy();

	void update();
	void draw();

};

