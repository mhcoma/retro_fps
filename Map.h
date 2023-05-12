#pragma once

#include <iostream>
#include <vector>

#include "glut.h"

#include "Manager.h"

enum BlockType {
	bt_underground,
	bt_ground,
	bt_wall,
	bt_enemy,
	bt_half
};

class Manager;
class Map {
public:
	int map_size = 32;
	int map[32][32] = {0, };

	Manager* manager;

	Map() {}
	~Map() {}

	bool load();
	void draw();

	void draw_underground_wall();
	void draw_ground(int height, int texture);
	void draw_ceil();
	void draw_wall(int height);

	inline int actor_pos_to_map_coord(double n) {
		return (int) ((n + this->map_size + 1) / 2);
	}
	inline double map_coord_to_actor_pos(int n) {
		return (n * 2) - 1 - this->map_size;
	}
};