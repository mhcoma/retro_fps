#pragma once

#include "Actor.h"

#include "glm.hpp"
#include "gtx/rotate_vector.hpp"
#include "gtx/vector_angle.hpp"
#include "ext/scalar_constants.hpp"

class Camera;
class CamActor : public Actor {
public:
	~CamActor();

	Camera* cam;

	glm::dvec3 muzzle_pos;

	double interval;
	
	double shoot_time_count = 0.0;

	glm::dvec3 get_camera_pos();
	glm::dvec3 get_camera_center();

	void shoot(bool enemys_bullet = false);
};

