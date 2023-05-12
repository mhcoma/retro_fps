#pragma once

#include <iostream>
#include <vector>

#include "glut.h"

#include "Coord.h"

#include "glm.hpp"

enum ActorType {
	at_none,
	at_player,
	at_enemy,
	at_bullet
};

class Manager;
class Map;

class Actor {
public:
	glm::dvec3 pos;
	glm::dvec3 size;
	glm::dvec3 euler_angle;

	Manager* manager;
	Map* map;

	int type;
	bool already_under_ground = false;
	int life;

	Actor();
	~Actor();
	
	virtual void update();
	virtual void draw();

	static bool actor_distance_comp(Actor* a, Actor* b);
};