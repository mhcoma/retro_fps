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
	//모든 바닥에 대해 충돌 체크를 할 필요가 없음
	int test_x = map->actor_pos_to_map_coord(this->pos.x);
	int max_test_x = std::min(test_x + 1, this->map->map_size - 1);
	int min_text_x = std::max(test_x - 1, 0);
	int test_z = map->actor_pos_to_map_coord(this->pos.z);
	int max_test_z = std::min(test_z + 1, this->map->map_size - 1);
	int min_text_z = std::max(test_z - 1, 0);

	bool under_ground = true;

	//플레이어가 떠 있을 경우, y축 속도는 중력의 영향을 받아 변화함
	if (this->pos.y > 0) {
		y_speed -= this->manager->gravity * this->manager->delta_time;
		under_ground = false;
	}
	//플레이어가 떠 있지 않을 경우
	else {
		//구역 내의 바닥과의 충돌 처리
		//이미 땅의 높이보다 낮으면 바닥에 닿지 않은 것으로 취급
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

						//플레이어와 바닥 모두 사각형으로 구성
						//조건에 맞지 않는 경우 넘어감
						if (player_max.x <= block_min.x || player_min.x >= block_max.x) continue;
						if (player_max.z <= block_min.z || player_min.z >= block_max.z) continue;

						//하나의 바닥이라도 충돌하면 지면 위에 있음
						under_ground = false;
						break;
					}
				}
			}
		}

		//땅에 닿아있다면 y축 위치를 고정함
		//이 떄, 점프 키가 눌리면 상승시킴
		if (!under_ground) {
			this->pos.y = 0;
			if (this->movement_state[4]) this->y_speed = 0.0625;
			else this->y_speed = 0;
		}
		//땅에 닿아있지 않다면 계속 추락시킴
		//이미 땅의 높이보다 낮음을 저장함
		else {
			this->y_speed -= this->manager->gravity * this->manager->delta_time;
			this->already_under_ground = true;

			//맵 밖을 벗어나지 않도록 최소 추락 높이를 설정
			if (this->pos.y < -4) {
				this->pos.y = -4;
				this->y_speed = 0;
				this->life = 0;
			}
		}
	}

	//마우스로 얻은 정보로부터 카메라는 x축, 플레이어는 y축 회전
	glm::dvec3 translation_vec = glm::dvec3(0, 0, 0);
	glm::dvec3 rotation_vec = glm::dvec3(0, this->camera_rotation[0], 0);
	glm::dvec3 cam_rotation_vec = glm::dvec3(this->camera_rotation[1], 0, 0);

	this->euler_angle += rotation_vec;
	this->cam->euler_angle += cam_rotation_vec;

	//카메라 pitch 제한
	if (this->cam->euler_angle.x >= cam_euler_x_limit)
		this->cam->euler_angle.x = cam_euler_x_limit;

	if (this->cam->euler_angle.x <= -cam_euler_x_limit)
		this->cam->euler_angle.x = -cam_euler_x_limit;

	//입력받은 축으로 이동
	if (this->movement_state[0])
		translation_vec += glm::dvec3(0, 0, 1);
	if (this->movement_state[1])
		translation_vec += glm::dvec3(0, 0, -1);
	if (this->movement_state[2])
		translation_vec += glm::dvec3(1, 0, 0);
	if (this->movement_state[3])
		translation_vec += glm::dvec3(-1, 0, 0);

	//대각선 이동에도 이동 길이가 동일하도록 벡터 정규화
	//길이가 0일 때는 정규화 에러가 발생하고, 움직이지도 않으니 처리하지 않음
	if (glm::length(translation_vec) != 0.0)
		translation_vec = glm::normalize(translation_vec) * (this->speed * this->manager->delta_time);

	//플레이어의 yaw 방향으로 이동
	//위의 정규화 등은 x축과 z축을 이용하기 때문에, y축 정보는 나중에 추가
	translation_vec = glm::rotateY(translation_vec, this->euler_angle.y);
	translation_vec.y = y_speed;
	glm::dvec3 next_pos = this->pos + translation_vec;


	//벽과의 충돌 체크 및 위치 조정
	std::vector<glm::dvec3> solution_list;
	std::vector<glm::dvec3> alt_solution_list;
	std::vector<Coord> collision_blocks;
	solution_list.reserve(3);
	alt_solution_list.reserve(3);
	collision_blocks.reserve(3);
	
	//모든 벽에 대해 충돌 체크를 할 필요가 없음
	test_x = map->actor_pos_to_map_coord(next_pos.x);
	max_test_x = std::min(test_x + 1, this->map->map_size - 1);
	min_text_x = std::max(test_x - 1, 0);
	test_z = map->actor_pos_to_map_coord(next_pos.z);
	max_test_z = std::min(test_z + 1, this->map->map_size - 1);
	min_text_z = std::max(test_z - 1, 0);

	//구역 내의 벽과의 충돌 처리
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

				//플레이어와 블럭 모두 사각형으로 구성
				//조건에 맞지 않는 경우 넘어감
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

	//충돌한 블럭이 하나일 경우
	//가장 많이 들어간 축 하나에 대해서 충돌 처리
	if (solution_count == 1) {
		next_pos += solution_list[0];
	}
	//충돌한 블럭이 둘인 경우
	else if (solution_count == 2) {
		bool check = false;
		if ((collision_blocks[0].x == collision_blocks[1].x) ||
			(collision_blocks[0].y == collision_blocks[1].y)) {
			check = true;
		}
		//두 블럭이 맞닿아 배치돼있을 경우
		//더 많이 충돌한 블럭에 대해서만 가장 많이 들어간 축 하나에 대해서 충돌 처리
		if (check) {
			if (glm::length(solution_list[0]) > glm::length(solution_list[1])) {
				next_pos += solution_list[0];
			}
			else {
				next_pos += solution_list[1];
			}
		}
		//두 블럭이 대각선으로 배치돼있을 경우
		//두 블럭 모두에 대해서 각각 가장 많이 들어간 축 하나에 대해서 충돌 처리
		else {
			next_pos += solution_list[0];
			next_pos += solution_list[1];
		}
	}
	//충돌한 블럭이 셋인 경우
	//ㄱ자 형식의 모서리 안쪽에 부딪힌 경우만 있음
	//안쪽 블럭에 대해서만 축 두개에 대해서 충돌 처리
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

	//최종적으로 계산된 플레이어의 위치를 반영
	this->pos = next_pos;

	//연사 시간 카운트
	this->shoot_time_count -= this->manager->delta_time;
	//발사 중일 때, 카운트가 되면 발사하고 연사 간격만큼 다시 더함
	if (this->shooting) {
		if (this->shoot_time_count < 0) {
			this->shoot();
			this->shoot_time_count += this->interval;
		}
	}
	//발사 중이 아니라면, 발사 카운트를 0으로 설정 후 대기
	//발사를 멈추고 연사 간격 이상의 시간이 지났을 때도 다시 발사하고자 하면 바로 발사됨
	else {
		if (this->shoot_time_count < 0) {
			this->shoot_time_count = 0;
		}
	}

	//죽었다면, 매니저로 하여금 리플레이
	if (this->life <= 0) {
		this->manager->replay();
	}
}