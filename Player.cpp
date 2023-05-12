#include "Player.h"

#include "Bullet.h"
#include "Camera.h"
#include "Manager.h"
#include "Map.h"

Player::Player() {
	this->cam = new Camera();

	this->pos = glm::dvec3(0.0, 0.0, 0.0);
	this->euler_angle = glm::dvec3(0, 0, 0);

	this->cam->pos = glm::dvec3(0.0, 2.0, 0.0);
	this->cam->euler_angle = glm::dvec3(0, 0, 0);

	this->muzzle_pos = glm::dvec3(0.0, 0.0, 0.5);
	this->size = glm::dvec3(0.5, 2, 0.5);

	this->interval = 0.125;
	this->type = ActorType::at_player;
	this->life = 10;
}

Player::~Player() {
}

void Player::draw_hud() {
	glEnable(GL_TEXTURE_2D);

	this->manager->translate_2d_pos_with_push(0.5, 0.5);
	glBindTexture(GL_TEXTURE_2D, this->manager->textures_indices[Texture::t_crosshair]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-64, 64, 0);
	glTexCoord2f(1, 1);
	glVertex3f(64, 64, 0);
	glTexCoord2f(1, 0);
	glVertex3f(64, -64, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-64, -64, 0);
	glEnd();
	glPopMatrix();


	double healthbar_tick_height = 64;
	double current_healthbar_height = healthbar_tick_height * (-this->life) - 32;
	double max_healthbar_height = healthbar_tick_height * (-10) - 32;

	this->manager->translate_2d_pos_with_push(1.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, this->manager->textures_indices[Texture::t_underground]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 10);
	glVertex3f(-96, -32, 0);
	glTexCoord2f(1, 10);
	glVertex3f(-32, -32, 0);
	glTexCoord2f(1, 0);
	glVertex3f(-32, max_healthbar_height, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-96, max_healthbar_height, 0);

	glEnd();

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0, this->life);
	glVertex3f(-96, -32, 0);
	glTexCoord2f(1, this->life);
	glVertex3f(-32, -32, 0);
	glTexCoord2f(1, 0);
	glVertex3f(-32, current_healthbar_height, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-96, current_healthbar_height, 0);
	glEnd();
	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_TEXTURE_2D);
}

void Player::update() {
	//��� �ٴڿ� ���� �浹 üũ�� �� �ʿ䰡 ����
	int test_x = map->actor_pos_to_map_coord(this->pos.x);
	int max_test_x = std::min(test_x + 1, this->map->map_size - 1);
	int min_text_x = std::max(test_x - 1, 0);
	int test_z = map->actor_pos_to_map_coord(this->pos.z);
	int max_test_z = std::min(test_z + 1, this->map->map_size - 1);
	int min_text_z = std::max(test_z - 1, 0);

	bool under_ground = true;

	//�÷��̾ �� ���� ���, y�� �ӵ��� �߷��� ������ �޾� ��ȭ��
	if (this->pos.y > 0) {
		y_speed -= this->manager->gravity * this->manager->delta_time;
		under_ground = false;
	}
	//�÷��̾ �� ���� ���� ���
	else {
		//���� ���� �ٴڰ��� �浹 ó��
		//�̹� ���� ���̺��� ������ �ٴڿ� ���� ���� ������ ���
		if (!this->already_under_ground) {
			for (int i = min_text_x; i <= max_test_x; i++) {
				for (int j = min_text_z; j <= max_test_z; j++) {
					if (this->map->map[i][j] == 1) {
						Coord block = {i, j};

						glm::dvec3 block_pos = glm::dvec3(i * 2 - this->map->map_size, 0, j * 2 - this->map->map_size);
						glm::dvec3 block_size = glm::dvec3(1, 0, 1);
						glm::dvec3 block_max = block_pos + block_size;
						glm::dvec3 block_min = block_pos - block_size;

						glm::dvec3 player_max = this->pos + this->size;
						glm::dvec3 player_min = this->pos - this->size;

						//�÷��̾�� �ٴ� ��� �簢������ ����
						//���ǿ� ���� �ʴ� ��� �Ѿ
						if (player_max.x <= block_min.x || player_min.x >= block_max.x) continue;
						if (player_max.z <= block_min.z || player_min.z >= block_max.z) continue;

						//�ϳ��� �ٴ��̶� �浹�ϸ� ���� ���� ����
						under_ground = false;
						break;
					}
				}
			}
		}

		//���� ����ִٸ� y�� ��ġ�� ������
		//�� ��, ���� Ű�� ������ ��½�Ŵ
		if (!under_ground) {
			this->pos.y = 0;
			if (this->movement_state[4]) this->y_speed = 0.0625;
			else this->y_speed = 0;
		}
		//���� ������� �ʴٸ� ��� �߶���Ŵ
		//�̹� ���� ���̺��� ������ ������
		else {
			this->y_speed -= this->manager->gravity * this->manager->delta_time;
			this->already_under_ground = true;

			//�� ���� ����� �ʵ��� �ּ� �߶� ���̸� ����
			if (this->pos.y < -4) {
				this->pos.y = -4;
				this->y_speed = 0;
				this->life = 0;
			}
		}
	}

	//���콺�� ���� �����κ��� ī�޶�� x��, �÷��̾�� y�� ȸ��
	glm::dvec3 translation_vec = glm::dvec3(0, 0, 0);
	glm::dvec3 rotation_vec = glm::dvec3(0, this->camera_rotation[0], 0);
	glm::dvec3 cam_rotation_vec = glm::dvec3(this->camera_rotation[1], 0, 0);

	this->euler_angle += rotation_vec;
	this->cam->euler_angle += cam_rotation_vec;

	//ī�޶� pitch ����
	if (this->cam->euler_angle.x >= cam_euler_x_limit)
		this->cam->euler_angle.x = cam_euler_x_limit;

	if (this->cam->euler_angle.x <= -cam_euler_x_limit)
		this->cam->euler_angle.x = -cam_euler_x_limit;

	//�Է¹��� ������ �̵�
	if (this->movement_state[0])
		translation_vec += glm::dvec3(0, 0, 1);
	if (this->movement_state[1])
		translation_vec += glm::dvec3(0, 0, -1);
	if (this->movement_state[2])
		translation_vec += glm::dvec3(1, 0, 0);
	if (this->movement_state[3])
		translation_vec += glm::dvec3(-1, 0, 0);

	//�밢�� �̵����� �̵� ���̰� �����ϵ��� ���� ����ȭ
	//���̰� 0�� ���� ����ȭ ������ �߻��ϰ�, ���������� ������ ó������ ����
	if (glm::length(translation_vec) != 0.0)
		translation_vec = glm::normalize(translation_vec) * (this->speed * this->manager->delta_time);

	//�÷��̾��� yaw �������� �̵�
	//���� ����ȭ ���� x��� z���� �̿��ϱ� ������, y�� ������ ���߿� �߰�
	translation_vec = glm::rotateY(translation_vec, this->euler_angle.y);
	translation_vec.y = y_speed;
	glm::dvec3 next_pos = this->pos + translation_vec;


	//������ �浹 üũ �� ��ġ ����
	std::vector<glm::dvec3> solution_list;
	std::vector<glm::dvec3> alt_solution_list;
	std::vector<Coord> collision_blocks;
	solution_list.reserve(3);
	alt_solution_list.reserve(3);
	collision_blocks.reserve(3);
	
	//��� ���� ���� �浹 üũ�� �� �ʿ䰡 ����
	test_x = map->actor_pos_to_map_coord(next_pos.x);
	max_test_x = std::min(test_x + 1, this->map->map_size - 1);
	min_text_x = std::max(test_x - 1, 0);
	test_z = map->actor_pos_to_map_coord(next_pos.z);
	max_test_z = std::min(test_z + 1, this->map->map_size - 1);
	min_text_z = std::max(test_z - 1, 0);

	//���� ���� ������ �浹 ó��
	for (int i = min_text_x; i <= max_test_x; i++) {
		for (int j = min_text_z; j <= max_test_z; j++) {
			if (this->map->map[i][j] > (under_ground ? 0 : 1)) {
				Coord block = {i, j};
				glm::dvec3 solution = glm::dvec3(0);
				glm::dvec3 alt_solution = glm::dvec3(0);

				glm::dvec3 block_pos = glm::dvec3(i * 2 - this->map->map_size, 0, j * 2 - this->map->map_size);
				glm::dvec3 block_size = glm::dvec3(1, 0, 1);
				glm::dvec3 block_max = block_pos + block_size;
				glm::dvec3 block_min = block_pos - block_size;

				glm::dvec3 player_max = next_pos + this->size;
				glm::dvec3 player_min = next_pos - this->size;

				//�÷��̾�� �� ��� �簢������ ����
				//���ǿ� ���� �ʴ� ��� �Ѿ
				if (player_max.x < block_min.x || player_min.x > block_max.x) continue;
				if (player_max.z < block_min.z || player_min.z > block_max.z) continue;

				bool condition_x = next_pos.x > block_pos.x;
				bool condition_z = next_pos.z > block_pos.z;

				if (abs(next_pos.x - block_pos.x) > abs(next_pos.z - block_pos.z)) {
					if (condition_x) solution.x += block_max.x - player_min.x;
					else solution.x -= player_max.x - block_min.x;
					if (condition_z) alt_solution.z += block_max.z - player_min.z;
					else alt_solution.z -= player_max.z - block_min.z;
				}
				else {
					if (condition_z) solution.z += block_max.z - player_min.z;
					else solution.z -= player_max.z - block_min.z;
					if (condition_x) alt_solution.x += block_max.x - player_min.x;
					else alt_solution.x -= player_max.x - block_min.x;
				}
				solution_list.push_back(solution);
				alt_solution_list.push_back(alt_solution);
				collision_blocks.push_back(block);
			}
		}
	}

	int solution_count = solution_list.size();

	//�浹�� ���� �ϳ��� ���
	//���� ���� �� �� �ϳ��� ���ؼ� �浹 ó��
	if (solution_count == 1) {
		next_pos += solution_list[0];
	}
	//�浹�� ���� ���� ���
	else if (solution_count == 2) {
		bool check = false;
		if ((collision_blocks[0].x == collision_blocks[1].x) ||
			(collision_blocks[0].y == collision_blocks[1].y)) {
			check = true;
		}
		//�� ���� �´�� ��ġ������ ���
		//�� ���� �浹�� ���� ���ؼ��� ���� ���� �� �� �ϳ��� ���ؼ� �浹 ó��
		if (check) {
			if (glm::length(solution_list[0]) > glm::length(solution_list[1])) {
				next_pos += solution_list[0];
			}
			else {
				next_pos += solution_list[1];
			}
		}
		//�� ���� �밢������ ��ġ������ ���
		//�� �� ��ο� ���ؼ� ���� ���� ���� �� �� �ϳ��� ���ؼ� �浹 ó��
		else {
			next_pos += solution_list[0];
			next_pos += solution_list[1];
		}
	}
	//�浹�� ���� ���� ���
	//���� ������ �𼭸� ���ʿ� �ε��� ��츸 ����
	//���� ���� ���ؼ��� �� �ΰ��� ���ؼ� �浹 ó��
	else if (solution_count == 3) {
		bool check[2] = {false, };
		Coord temp = collision_blocks[0];
		int index;
		for (int i = 1; i < 3; i++) {
			if ((collision_blocks[i].x == collision_blocks[0].x) ||
				(collision_blocks[i].y == collision_blocks[0].y)) {
				check[i - 1] = true;
			}
		}
		index = (check[0] == check[1]) ? 0 : (check[0] ? 1 : 2);
		next_pos += solution_list[index];
		next_pos += alt_solution_list[index];
	}

	//���������� ���� �÷��̾��� ��ġ�� �ݿ�
	this->pos = next_pos;

	//���� �ð� ī��Ʈ
	this->shoot_time_count -= this->manager->delta_time;
	//�߻� ���� ��, ī��Ʈ�� �Ǹ� �߻��ϰ� ���� ���ݸ�ŭ �ٽ� ����
	if (this->shooting) {
		if (this->shoot_time_count < 0) {
			this->shoot();
			this->shoot_time_count += this->interval;
		}
	}
	//�߻� ���� �ƴ϶��, �߻� ī��Ʈ�� 0���� ���� �� ���
	//�߻縦 ���߰� ���� ���� �̻��� �ð��� ������ ���� �ٽ� �߻��ϰ��� �ϸ� �ٷ� �߻��
	else {
		if (this->shoot_time_count < 0) {
			this->shoot_time_count = 0;
		}
	}

	//�׾��ٸ�, �Ŵ����� �Ͽ��� ���÷���
	if (this->life <= 0) {
		this->manager->replay();
	}
}