#include "Manager.h"

#include "Actor.h"
#include "Bullet.h"
#include "Map.h"
#include "Player.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//�ҷ��� �ؽ�ó ���ϸ���� �迭
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

//�Ŵ����� ���� ���� �ʱ�ȭ
Manager::Manager() {
	this->screen_size = {1920, 1080};
	this->last_mouse = {screen_size.x / 2, screen_size.y / 2};
	this->diff_mouse = {0, 0};
	this->delta_time = 0;

	this->player = NULL;
	this->map = NULL;
}

//�Ŵ��� �Ҹ� �� �����Ҵ�� ��ü�� �Ҵ� ����
Manager::~Manager() {
	this->remove();
}

void Manager::init() {
	//�÷��̾�� �� ��ü ����
	this->player = new Player();
	this->map = new Map();

	this->player->pos = glm::dvec3(0, 0, 0);
	this->player->map = this->map;
	this->player->manager = this;

	this->map->manager = this;
	this->map->load();

	//���� �ð� ����
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

	//�ҷ��� �ؽ�ó�� ������ �ؽ�ó ���ϸ� �迭�� �ľ�
	int count = sizeof(this->textures_names) / sizeof(*this->textures_names);

	//�ݺ����� ���鼭 �ؽ�ó �ε�
	for (int i = 0; i < count; i++) {
		//stbi�� �ؽ�ó �ε�
		unsigned char* data = stbi_load(textures_names[i], &width, &height, &channel, 0);
		if (!data) return false;

		GLuint index;
		glGenTextures(1, &index);
		glBindTexture(GL_TEXTURE_2D, index);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		//�ִ����� ���ø� ���
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//����� �ؽ�ó�� �ݺ�
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);

		//�ؽ�ó �ε����� �迭�� �߰�
		//�ؽ�ó ���ϸ� �迭�� ������ �ؽ�ó ���� üũ�� Enum�� ������ ����
		//���� ������� �߰��ϱ� ������ Enum�� ������ �ؽ�ó�� ����� �� ����
		textures_indices.push_back(index);
		
		stbi_image_free(data);
	}

	return true;
}

void Manager::update() {
	//��Ÿ Ÿ�� ���
	this->current_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> delta = (this->current_time - this->old_time);
	this->delta_time = delta.count();
	this->old_time = this->current_time;

	//�÷��̾� ��ü ������Ʈ
	this->player->update();

	//��� ��ü�� �ݺ������� ������Ʈ
	for (auto iter = this->actors.begin(); iter != this->actors.end(); iter++) {
		(*iter)->update();

		//���� ��ü�� �׾��ٸ�, ��ü�� ������
		if (!((*iter)->life)) {
			delete (*iter);
			this->actors.erase(iter--);
		}
	}
}

void Manager::draw() {
	//���� �׸�
	this->map->draw();

	//��ü���� �÷��̾���� �Ÿ��� ���� ���ĵǾ� ����
	//����, �������� ��ü�� ���ĵǾ� ������
	this->actors.sort(Actor::actor_distance_comp);
	//���� �Ϸ� �� ��� ��ü�� ������� �׸�
	for (auto iter = this->actors.begin(); iter != this->actors.end(); iter++) {
		(*iter)->draw();
	}
}

void Manager::draw_hud() {
	//HUD�� ���� �׽�Ʈ�� ���� ����
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	//�������ǰ� �𵨺並 �̸� Ǫ�� �� �ʱ�ȭ�ϰ�,
	//ȭ�鿡 2D ������� �׸����� ����
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, this->screen_size.x, this->screen_size.y, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//�÷��̾��� HUD �׸���
	this->player->draw_hud();

	//���󺹱�
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void Manager::translate_2d_pos_with_push(double x, double y) {
	//Ǫ�ÿ� �Բ�, ȭ���� Ư�� ��ġ�� �������� �̵�
	glPushMatrix();
	glTranslatef(this->screen_size.x * x, this->screen_size.y * y, 0);
}