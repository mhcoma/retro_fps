#include "Manager.h"

#include "Actor.h"
#include "Bullet.h"
#include "Map.h"
#include "Player.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//불러올 텍스처 파일명들의 배열
const char* Manager::textures_names[] = {
	"image\\underground.png",
	"image\\ground.png",
	"image\\wall.png",
	"image\\ceil.png",
	"image\\bullet.png",
	"image\\enemy.png",
	"image\\enemy_bullet.png",
	"image\\crosshair.png"
};

//매니저가 갖는 정보 초기화
Manager::Manager() {
	this->screen_size = {1920, 1080};
	this->last_mouse = {screen_size.x / 2, screen_size.y / 2};
	this->diff_mouse = {0, 0};
	this->delta_time = 0;

	this->player = NULL;
	this->map = NULL;
}

//매니저 소멸 시 동적할당된 객체들 할당 해제
Manager::~Manager() {
	this->remove();
}

void Manager::init() {
	//플레이어와 맵 객체 생성
	this->player = new Player();
	this->map = new Map();

	this->player->pos = glm::dvec3(0, 0, 0);
	this->player->map = this->map;
	this->player->manager = this;

	this->map->manager = this;
	this->map->load();

	//측정 시간 설정
	this->old_time = std::chrono::high_resolution_clock::now();
}

void Manager::remove() {
	delete this->player;
	delete this->map;

	for (auto iter = this->actors.begin(); iter != this->actors.end(); iter++) {
		delete (*iter);
	}
	this->actors.clear();
}

void Manager::replay() {
	this->remove();
	this->init();
}

bool Manager::load_textures() {
	int width, height, channel;

	//불러올 텍스처의 개수는 텍스처 파일명 배열로 파악
	int count = sizeof(this->textures_names) / sizeof(*this->textures_names);

	//반복문을 돌면서 텍스처 로드
	for (int i = 0; i < count; i++) {
		//stbi로 텍스처 로드
		unsigned char* data = stbi_load(textures_names[i], &width, &height, &channel, 0);
		if (!data) return false;

		GLuint index;
		glGenTextures(1, &index);
		glBindTexture(GL_TEXTURE_2D, index);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		//최단입점 샘플링 사용
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//벗어나는 텍스처는 반복
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

		//텍스처 인덱스를 배열에 추가
		//텍스처 파일명 배열의 순서와 텍스처 파일 체크용 Enum의 순서가 같고
		//또한 순서대로 추가하기 때문에 Enum의 값으로 텍스처를 사용할 수 있음
		textures_indices.push_back(index);
		
		stbi_image_free(data);
	}

	return true;
}

void Manager::update() {
	//델타 타임 계산
	this->current_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> delta = (this->current_time - this->old_time);
	this->delta_time = delta.count();
	this->old_time = this->current_time;

	//플레이어 객체 업데이트
	this->player->update();

	//모든 객체를 반복문으로 업데이트
	for (auto iter = this->actors.begin(); iter != this->actors.end(); iter++) {
		(*iter)->update();

		//만약 객체가 죽었다면, 객체를 삭제함
		if (!((*iter)->life)) {
			delete (*iter);
			this->actors.erase(iter--);
		}
	}
}

void Manager::draw() {
	//맵을 그림
	this->map->draw();

	//객체들은 플레이어와의 거리에 따라 정렬되어 보임
	//따라서, 반투명한 객체가 정렬되어 보여짐
	this->actors.sort(Actor::actor_distance_comp);
	//정렬 완료 후 모든 객체를 순서대로 그림
	for (auto iter = this->actors.begin(); iter != this->actors.end(); iter++) {
		(*iter)->draw();
	}
}

void Manager::draw_hud() {
	//HUD는 깊이 테스트를 하지 않음
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	//프로젝션과 모델뷰를 미리 푸시 후 초기화하고,
	//화면에 2D 방식으로 그리도록 지정
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, this->screen_size.x, this->screen_size.y, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//플레이어의 HUD 그리기
	this->player->draw_hud();

	//원상복구
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void Manager::translate_2d_pos_with_push(double x, double y) {
	//푸시와 함께, 화면의 특정 위치를 기준으로 이동
	glPushMatrix();
	glTranslatef(this->screen_size.x * x, this->screen_size.y * y, 0);
}