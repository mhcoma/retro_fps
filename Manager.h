#pragma once

#include <chrono>
#include <list>
#include <vector>

#include "glut.h"

#include "Coord.h"

enum Texture {
	t_underground,
	t_ground,
	t_wall,
	t_ceil,
	t_bullet,
	t_enemy,
	t_enemy_bullet,
	t_crosshair
};

class Actor;
class Bullet;
class Map;
class Player;

class Manager {
public:
	Manager();
	~Manager();

	Player* player;
	Map* map;
	std::list<Actor*> actors;

	static const char* textures_names[8];
	std::vector<GLuint> textures_indices;

	std::chrono::steady_clock::time_point current_time, old_time;
	double delta_time;
	double gravity = 0.1875;

	Coord screen_size;
	Coord last_mouse;
	Coord diff_mouse;

	void init();
	void remove();
	void replay();

	bool load_textures();

	void update();
	void draw();
	void draw_hud();

	void translate_2d_pos_with_push(double x, double y);
};

