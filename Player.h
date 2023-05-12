#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

#include "glm.hpp"
#include "gtx/string_cast.hpp"
#include "gtx/rotate_vector.hpp"
#include "gtx/vector_angle.hpp"
#include "ext/scalar_constants.hpp"

#include "CamActor.h"
#include "Actor.h"

static double cam_euler_x_limit = glm::half_pi<double>() - 0.001;

class Player : public CamActor {
public:
	bool movement_state[5] = {false, };
	double camera_rotation[2] = {0.0, };
	double speed = 8.75;
	double y_speed = 0.0;

	bool shooting = false;


	Player();
	~Player();
	void draw_hud();
	void update();
};