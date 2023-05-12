#include "CamActor.h"

#include "Bullet.h"
#include "Camera.h"
#include "Manager.h"
#include "Map.h"

CamActor::~CamActor() {
	delete this->cam;
}

glm::dvec3 CamActor::get_camera_pos() {
	//������ ��ġ�� ī�޶��� local ��ġ�� ���� ���� ���� ī�޶��� ��ġ
	return this->pos + this->cam->pos;
}

glm::dvec3 CamActor::get_camera_center() {
	//ī�޶��� pitch�� ������ yaw�� ȸ���� ���� ī�޶� �߽����� ����
	glm::dvec3 value = glm::rotateX(glm::dvec3(0, 0, 1), this->cam->euler_angle.x);
	value = this->get_camera_pos() + glm::rotateY(value, this->euler_angle.y);
	return value;
}

void CamActor::shoot(bool enemys_bullet) {
	//�Ѿ��� ����
	Bullet* bullet = new Bullet();
	bullet->manager = this->manager;
	bullet->map = this->map;

	//�Ѿ��� ���� ��ġ�� ���ư��� ������
	//�� �Ѿ��� �߻��ϴ� ������ ��ġ�� ����, ī�޶��� ������ �������� �����
	glm::dvec3 temp_pos = glm::rotateX(this->muzzle_pos, this->cam->euler_angle.x);
	temp_pos = glm::rotateY(temp_pos, this->euler_angle.y);
	bullet->pos = this->get_camera_pos() + temp_pos;
	bullet->euler_angle = glm::dvec3(this->cam->euler_angle.x, this->euler_angle.y, 0);

	//���� �߻��� ���
	bullet->is_enemys_bullet = enemys_bullet;

	bullet->type = ActorType::at_bullet;
	bullet->life = 1;

	//�Ŵ����� ���� ����Ʈ�� ������ �Ѿ��� �߰���
	this->manager->actors.push_back(bullet);
}