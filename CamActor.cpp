#include "CamActor.h"

#include "Bullet.h"
#include "Camera.h"
#include "Manager.h"
#include "Map.h"

CamActor::~CamActor() {
	delete this->cam;
}

glm::dvec3 CamActor::get_camera_pos() {
	//액터의 위치와 카메라의 local 위치를 더한 곳이 실제 카메라의 위치
	return this->pos + this->cam->pos;
}

glm::dvec3 CamActor::get_camera_center() {
	//카메라의 pitch와 액터의 yaw로 회전한 곳을 카메라 중심으로 결정
	glm::dvec3 value = glm::rotateX(glm::dvec3(0, 0, 1), this->cam->euler_angle.x);
	value = this->get_camera_pos() + glm::rotateY(value, this->euler_angle.y);
	return value;
}

void CamActor::shoot(bool enemys_bullet) {
	//총알의 생성
	Bullet* bullet = new Bullet();
	bullet->manager = this->manager;
	bullet->map = this->map;

	//총알의 생성 위치와 나아가는 방향은
	//그 총알을 발사하는 액터의 위치와 각도, 카메라의 각도를 기준으로 계산함
	glm::dvec3 temp_pos = glm::rotateX(this->muzzle_pos, this->cam->euler_angle.x);
	temp_pos = glm::rotateY(temp_pos, this->euler_angle.y);
	bullet->pos = this->get_camera_pos() + temp_pos;
	bullet->euler_angle = glm::dvec3(this->cam->euler_angle.x, this->euler_angle.y, 0);

	//적이 발사한 경우
	bullet->is_enemys_bullet = enemys_bullet;

	bullet->type = ActorType::at_bullet;
	bullet->life = 1;

	//매니저의 액터 리스트에 생성한 총알을 추가함
	this->manager->actors.push_back(bullet);
}