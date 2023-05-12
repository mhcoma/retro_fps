#include "Enemy.h"

#include "Bullet.h"
#include "Camera.h"

Enemy::Enemy() {
	this->cam = new Camera();

	this->pos = glm::dvec3(0.0, 0.0, 0.0);
	this->euler_angle = glm::dvec3(0, 0, 0);

	this->cam->pos = glm::dvec3(0.0, 1.5, 0.0);
	this->cam->euler_angle = glm::dvec3(0, 0, 0);

	this->muzzle_pos = glm::dvec3(0.0, 0.0, 0.5);
	this->size = glm::dvec3(0.5, 2, 0.5);

	this->interval = 1.0;
	this->type = ActorType::at_enemy;
	this->life = 5;
}

Enemy::~Enemy() {

}

void Enemy::update() {
	//플레이어를 바라보는 방향 구하기
	glm::dvec3 rotation_vec = this->manager->player->pos - this->pos;
	rotation_vec.y = 0;
	glm::dvec3 cam_rotation_vec = glm::dvec3(
		-glm::length(rotation_vec),
		this->manager->player->pos.y - this->pos.y,
		0.0
	);

	//방향에 맞게 적과 적 카메라의 각도를 설정
	//적도 플레이어처럼 자신의 각도를 기준으로 발사함
	this->euler_angle.y = atan2(rotation_vec.x, rotation_vec.z) + glm::pi<double>();;
	this->cam->euler_angle.x = -atan2(cam_rotation_vec.y, cam_rotation_vec.x);

	//적은 항상 연사 속도에 맞추어 계속 발사함
	this->shoot_time_count -= this->manager->delta_time;
	if (this->shoot_time_count < 0) {
		this->shoot(true);
		this->shoot_time_count += this->interval;
	}
}

void Enemy::draw() {
	//적에 대해서는 광원 효과를 적용시키지 않음
	glDisable(GL_LIGHTING);
	//텍스처 사용
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	//적의 위치를 기준으로 그림
	glTranslatef(this->pos.x, this->pos.y, this->pos.z);
	//적의 회전을 기준으로 그림
	//이 회전 수준은 update에서 플레이어를 바라보는 각도로 정해짐
	glRotatef(glm::degrees(this->euler_angle.y), 0.0, 1.0, 0.0);

	//적 텍스처 바인드
	glBindTexture(GL_TEXTURE_2D, manager->textures_indices[Texture::t_enemy]);
	//평면을 그림 위의 회전에 따라서 플레이어를 y축 회전으로만 따라잡게 그려짐

	float red_level =  this->life / 5.0f;


	glColor3f(1.0f, red_level, red_level);

	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(1, 1);
	glVertex3f(-0.5, 0, 0);
	glTexCoord2f(1, 0);
	glVertex3f(-0.5, 2, 0);
	glTexCoord2f(0, 0);
	glVertex3f(0.5, 2, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0.5, 0, 0);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();

	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}