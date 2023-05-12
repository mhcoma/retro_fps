#include "Bullet.h"

#include "Enemy.h"
#include "Map.h"
#include "Manager.h"
#include "Player.h"

Bullet::Bullet() {

}

Bullet::~Bullet() {

}

void Bullet::update() {
	//�Ѿ��� ���ư��� ����, ���� z�������� �ӵ����� ����
	glm::dvec3 translation_vec = glm::dvec3(0, 0, this->speed * this->manager->delta_time * (this->is_enemys_bullet ? 0.5 : 1.0));

	//�̰��� �Ѿ��� ���⿡ ���缭 ȸ��
	translation_vec = glm::rotateX(translation_vec, this->euler_angle.x);
	translation_vec = glm::rotateY(translation_vec, this->euler_angle.y);

	//�Ѿ��� �ڽ��� ���⿡ �°� ���ư�
	this->pos += translation_vec;
	
	//�浹�� �ľ��ϱ� ���� ���� ������
	bool under_ground = true;
	bool collision = false;
	bool enemy_block = false;

	//õ�忡 ��Ҵٸ� �浹
	if (this->pos.y >= 4) {
		collision = true;
	}
	//õ��� �ٴ� ���̿� ���� ���� �浹�� �ƴ�
	else if (this->pos.y > 0) {
		under_ground = false;
		//���� ���� ��ħ ������ �浹 ���� ����
		if (this->pos.y <= 1) {
			enemy_block = true;
		}
	}
	//�ٴ� ���� ������ ��
	else {
		this->already_under_ground = true;
		//������ ���̺��ٵ� ������ �浹
		if (this->pos.y <= -4) collision = true;
	}

	//��� ��Ͽ� ���� �浹 ó���� �� �ʿ� ����
	int test_x = (int) (((this->pos.x) + this->map->map_size + 1) / 2);
	int max_test_x = std::min(test_x + 1, this->map->map_size - 1);
	int min_text_x = std::max(test_x - 1, 0);
	int test_z = (int) (((this->pos.z) + this->map->map_size + 1) / 2);
	int max_test_z = std::min(test_z + 1, this->map->map_size - 1);
	int min_text_z = std::max(test_z - 1, 0);

	//���� �浹���� �ʾ��� ���, ����� ���鸸�� ������� �浹 ����
	if (!collision) {
		for (int i = min_text_x; i <= max_test_x; i++) {
			for (int j = min_text_z; j <= max_test_z; j++) {
				//���� �浹 üũ�� �ؾ��ϴ� ���� ���
				if (this->collision_block(this->map->map[i][j], under_ground, enemy_block)) {
					Coord block = {i, j};

					//����� ���ο� �Ѿ��� ���� �浹 �����ϴ� ���
					glm::dvec3 block_pos = glm::dvec3(i * 2 - this->map->map_size, 0, j * 2 - this->map->map_size);
					glm::dvec3 block_size = glm::dvec3(1, 0, 1);
					glm::dvec3 block_max = block_pos + block_size;
					glm::dvec3 block_min = block_pos - block_size;

					//���ǿ� ���� �ʴ� ��� �Ѿ
					if (this->pos.x < block_min.x || this->pos.x > block_max.x) continue;
					if (this->pos.z < block_min.z || this->pos.z > block_max.z) continue;

					//���ǿ� �����ϸ� �浹 Ȯ��
					collision = true;
					break;
				}
			}
		}
	}

	//������ �浹 ó���� ���� �ʾ��� ���
	if (!collision) {
		//�Ѿ��� ���� �߻����� ���, �÷��̾ ���� �浹 ó��
		if (this->is_enemys_bullet) {
			Player* p = this->manager->player;

			//�÷��̾�� �� ��� 3���� ����ü�� ���
			//������ �߽����� �ٴ� �� ����� ����
			//X, Z�� ũ��� +, - �������� ���ư�
			//Y�� ũ��� �ٴڿ��� + �������θ� ���ư�
			glm::dvec3 player_max = p->pos + p->size;
			glm::dvec3 player_min = p->pos - p->size;
			player_min.y = p->pos.y;

			//�÷��̾�� �� ���, �Ѿ˰� �浹�ϸ� ������� ������
			if (!(this->pos.x < player_min.x || this->pos.x > player_max.x) &&
				!(this->pos.y < player_min.y || this->pos.y > player_max.y) &&
				!(this->pos.z < player_min.z || this->pos.z > player_max.z)
			) {
				p->life -= 1;
				collision = true;
			}
		}
		//�Ѿ��� �÷��̾ �߻����� ���, ������ �±װ��� ���� ���� ���Ϳ� ���ؼ��� �浹 ó��
		else {
			for (auto iter = this->manager->actors.begin(); iter != this->manager->actors.end(); iter++) {
				if ((*iter)->type == ActorType::at_enemy) {
					Enemy* e = (Enemy*) (*iter);
					glm::dvec3 enemy_max = e->pos + e->size;
					glm::dvec3 enemy_min = e->pos - e->size;
					enemy_min.y = e->pos.y;

					if (this->pos.x < enemy_min.x || this->pos.x > enemy_max.x) continue;
					if (this->pos.y < enemy_min.y || this->pos.y > enemy_max.y) continue;
					if (this->pos.z < enemy_min.z || this->pos.z > enemy_max.z) continue;

					e->life -= 1;
					collision = true;
					break;
				}
			}
		}
	}

	//�����̵� �����̵� �浹���� ��� �Ѿ��� �����
	if (collision) {
		this->life = 0;
	}
}

void Bullet::draw() {
	//�Ѿ˿� ���ؼ��� ���� ȿ���� �����Ű�� ����
	glDisable(GL_LIGHTING);
	//�ؽ�ó ���
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	//�Ѿ��� ��ġ�� �������� �׸�
	glTranslatef(this->pos.x, this->pos.y, this->pos.z);

	//�Ѿ��� �÷��̾ �ٶ󺸴� ���� ����� ���� ����
	glm::dvec3 texture_angle_vector_y = this->manager->player->pos - this->pos;
	texture_angle_vector_y.y = 0;
	glm::dvec3 texture_angle_vector_z = glm::dvec3(
		-glm::length(texture_angle_vector_y),
		this->manager->player->get_camera_pos().y - this->pos.y,
		0.0
	);
	
	//atan2�� �̿��� ���ͷκ��� ������ ����
	double texture_angle_y = atan2(texture_angle_vector_y.x, texture_angle_vector_y.z) - glm::half_pi<double>();
	double texture_angle_z = - atan2(texture_angle_vector_z.y, texture_angle_vector_z.x) + glm::half_pi<double>();

	//���� ������ ȸ��
	//���� ��� ���������� �Ѿ� ����� �÷��̾ �ٶ�
	glRotatef(glm::degrees(texture_angle_y), 0.0, 1.0, 0.0);
	glRotatef(glm::degrees(texture_angle_z), 0.0, 0.0, 1.0);

	//���� �Ѿ����� �÷��̾��� �Ѿ������� ���� �ٸ� �Ѿ� �ؽ�ó�� ���
	int texture_index;
	if (is_enemys_bullet)
		texture_index = Texture::t_enemy_bullet;
	else
		texture_index = Texture::t_bullet;
	glBindTexture(GL_TEXTURE_2D, manager->textures_indices[texture_index]);

	//�Ѿ��� ������� �׸�
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-0.25, 0, -0.25);
	glTexCoord2f(1, 0);
	glVertex3f(-0.25, 0, 0.25);
	glTexCoord2f(0, 0);
	glVertex3f(0.25, 0, 0.25);
	glTexCoord2f(0, 1);
	glVertex3f(0.25, 0, -0.25);

	glEnd();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

bool Bullet::collision_block(int block, bool under_ground, bool enemy_block) {
	//�浹 üũ�� �ؾ��ϴ��� Ȯ��
	//���Ͽ� �ִٸ� �ٴ� ���� ���ؼ��� �浹 üũ�� �ؾ���
	bool result = block > (under_ground ? 0 : 1);
	//���� ��� ���� �� ��ħ ��������
	//�� ��ħ ���� �浹���� �ʴ� ������ ��� �浹�� �ƴ�
	if ((block == bt_enemy || block == bt_half) && !enemy_block && !under_ground) result = false;
	return result;
}