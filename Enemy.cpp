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
	//�÷��̾ �ٶ󺸴� ���� ���ϱ�
	glm::dvec3 rotation_vec = this->manager->player->pos - this->pos;
	rotation_vec.y = 0;
	glm::dvec3 cam_rotation_vec = glm::dvec3(
		-glm::length(rotation_vec),
		this->manager->player->pos.y - this->pos.y,
		0.0
	);

	//���⿡ �°� ���� �� ī�޶��� ������ ����
	//���� �÷��̾�ó�� �ڽ��� ������ �������� �߻���
	this->euler_angle.y = atan2(rotation_vec.x, rotation_vec.z) + glm::pi<double>();;
	this->cam->euler_angle.x = -atan2(cam_rotation_vec.y, cam_rotation_vec.x);

	//���� �׻� ���� �ӵ��� ���߾� ��� �߻���
	this->shoot_time_count -= this->manager->delta_time;
	if (this->shoot_time_count < 0) {
		this->shoot(true);
		this->shoot_time_count += this->interval;
	}
}

void Enemy::draw() {
	//���� ���ؼ��� ���� ȿ���� �����Ű�� ����
	glDisable(GL_LIGHTING);
	//�ؽ�ó ���
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	//���� ��ġ�� �������� �׸�
	glTranslatef(this->pos.x, this->pos.y, this->pos.z);
	//���� ȸ���� �������� �׸�
	//�� ȸ�� ������ update���� �÷��̾ �ٶ󺸴� ������ ������
	glRotatef(glm::degrees(this->euler_angle.y), 0.0, 1.0, 0.0);

	//�� �ؽ�ó ���ε�
	glBindTexture(GL_TEXTURE_2D, manager->textures_indices[Texture::t_enemy]);
	//����� �׸� ���� ȸ���� ���� �÷��̾ y�� ȸ�����θ� ������� �׷���

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