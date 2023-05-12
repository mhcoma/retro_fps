#include "Actor.h"

#include "Map.h"
#include "Player.h"

Actor::Actor() {
	this->pos = glm::dvec3(0);
	this->size = glm::dvec3(0);
	this->euler_angle = glm::dvec3(0, 1, 0);

	this->manager = NULL;
	this->map = NULL;
	
	this->type = ActorType::at_none;
	this->life = 0;
}

Actor::~Actor() {

}

void Actor::update() {

}
void Actor::draw() {

}

bool Actor::actor_distance_comp(Actor* a, Actor* b) {
	//두 액터의 플레이어 카메라와의 거리 비교
	Player* player = a->manager->player;
	double a_distance = glm::length(a->pos - player->get_camera_pos());
	double b_distance = glm::length(b->pos - player->get_camera_pos());
	return a_distance > b_distance;
}