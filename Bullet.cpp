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
	//총알이 나아가는 벡터, 아직 z축으로의 속도값만 있음
	glm::dvec3 translation_vec = glm::dvec3(0, 0, this->speed * this->manager->delta_time * (this->is_enemys_bullet ? 0.5 : 1.0));

	//이것을 총알의 방향에 맞춰서 회전
	translation_vec = glm::rotateX(translation_vec, this->euler_angle.x);
	translation_vec = glm::rotateY(translation_vec, this->euler_angle.y);

	//총알은 자신의 방향에 맞게 나아감
	this->pos += translation_vec;
	
	//충돌을 파악하기 위한 지역 변수들
	bool under_ground = true;
	bool collision = false;
	bool enemy_block = false;

	//천장에 닿았다면 충돌
	if (this->pos.y >= 4) {
		collision = true;
	}
	//천장과 바닥 사이에 있을 때엔 충돌이 아님
	else if (this->pos.y > 0) {
		under_ground = false;
		//적이 놓인 받침 공간의 충돌 높이 여부
		if (this->pos.y <= 1) {
			enemy_block = true;
		}
	}
	//바닥 높이 이하일 때
	else {
		this->already_under_ground = true;
		//함정의 높이보다도 낮으면 충돌
		if (this->pos.y <= -4) collision = true;
	}

	//모든 블록에 대해 충돌 처리를 할 필요 없음
	int test_x = (int) (((this->pos.x) + this->map->map_size + 1) / 2);
	int max_test_x = std::min(test_x + 1, this->map->map_size - 1);
	int min_text_x = std::max(test_x - 1, 0);
	int test_z = (int) (((this->pos.z) + this->map->map_size + 1) / 2);
	int max_test_z = std::min(test_z + 1, this->map->map_size - 1);
	int min_text_z = std::max(test_z - 1, 0);

	//아직 충돌하지 않았을 경우, 가까운 블럭들만을 대상으로 충돌 감지
	if (!collision) {
		for (int i = min_text_x; i <= max_test_x; i++) {
			for (int j = min_text_z; j <= max_test_z; j++) {
				//블럭이 충돌 체크를 해야하는 블럭일 경우
				if (this->collision_block(this->map->map[i][j], under_ground, enemy_block)) {
					Coord block = {i, j};

					//블록의 내부에 총알이 들어가면 충돌 감지하는 방식
					glm::dvec3 block_pos = glm::dvec3(i * 2 - this->map->map_size, 0, j * 2 - this->map->map_size);
					glm::dvec3 block_size = glm::dvec3(1, 0, 1);
					glm::dvec3 block_max = block_pos + block_size;
					glm::dvec3 block_min = block_pos - block_size;

					//조건에 맞지 않는 경우 넘어감
					if (this->pos.x < block_min.x || this->pos.x > block_max.x) continue;
					if (this->pos.z < block_min.z || this->pos.z > block_max.z) continue;

					//조건에 충족하면 충돌 확인
					collision = true;
					break;
				}
			}
		}
	}

	//여전히 충돌 처리가 되지 않았을 경우
	if (!collision) {
		//총알을 적이 발사했을 경우, 플레이어에 대해 충돌 처리
		if (this->is_enemys_bullet) {
			Player* p = this->manager->player;

			//플레이어와 적 모두 3차원 육면체의 모습
			//액터의 중심점은 바닥 한 가운데에 있음
			//X, Z축 크기는 +, - 방향으로 나아감
			//Y축 크기는 바닥에서 + 방향으로만 나아감
			glm::dvec3 player_max = p->pos + p->size;
			glm::dvec3 player_min = p->pos - p->size;
			player_min.y = p->pos.y;

			//플레이어와 적 모두, 총알과 충돌하면 생명력이 감소함
			if (!(this->pos.x < player_min.x || this->pos.x > player_max.x) &&
				!(this->pos.y < player_min.y || this->pos.y > player_max.y) &&
				!(this->pos.z < player_min.z || this->pos.z > player_max.z)
			) {
				p->life -= 1;
				collision = true;
			}
		}
		//총알을 플레이어가 발사했을 경우, 일종의 태그값을 비교해 적인 액터에 대해서만 충돌 처리
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

	//지형이든 액터이든 충돌했을 경우 총알은 사라짐
	if (collision) {
		this->life = 0;
	}
}

void Bullet::draw() {
	//총알에 대해서는 광원 효과를 적용시키지 않음
	glDisable(GL_LIGHTING);
	//텍스처 사용
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	//총알의 위치를 기준으로 그림
	glTranslatef(this->pos.x, this->pos.y, this->pos.z);

	//총알이 플레이어를 바라보는 각도 계산을 위한 벡터
	glm::dvec3 texture_angle_vector_y = this->manager->player->pos - this->pos;
	texture_angle_vector_y.y = 0;
	glm::dvec3 texture_angle_vector_z = glm::dvec3(
		-glm::length(texture_angle_vector_y),
		this->manager->player->get_camera_pos().y - this->pos.y,
		0.0
	);
	
	//atan2를 이용해 벡터로부터 각도를 구함
	double texture_angle_y = atan2(texture_angle_vector_y.x, texture_angle_vector_y.z) - glm::half_pi<double>();
	double texture_angle_z = - atan2(texture_angle_vector_z.y, texture_angle_vector_z.x) + glm::half_pi<double>();

	//구한 각도로 회전
	//따라서 어떠한 각도에서도 총알 평면은 플레이어를 바라봄
	glRotatef(glm::degrees(texture_angle_y), 0.0, 1.0, 0.0);
	glRotatef(glm::degrees(texture_angle_z), 0.0, 0.0, 1.0);

	//적의 총알인지 플레이어의 총알인지에 따라서 다른 총알 텍스처를 사용
	int texture_index;
	if (is_enemys_bullet)
		texture_index = Texture::t_enemy_bullet;
	else
		texture_index = Texture::t_bullet;
	glBindTexture(GL_TEXTURE_2D, manager->textures_indices[texture_index]);

	//총알은 평면으로 그림
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
	//충돌 체크를 해야하는지 확인
	//지하에 있다면 바닥 블럭에 대해서도 충돌 체크를 해야함
	bool result = block > (under_ground ? 0 : 1);
	//만약 대상 블럭이 적 받침 블럭이지만
	//적 받침 블럭과 충돌하지 않는 높이일 경우 충돌이 아님
	if ((block == bt_enemy || block == bt_half) && !enemy_block && !under_ground) result = false;
	return result;
}