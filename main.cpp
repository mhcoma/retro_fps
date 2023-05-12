#include <iostream>
#include <vector>

#include "glut.h"

#include "Coord.h"
#include "Manager.h"
#include "Map.h"
#include "Player.h"

Manager manager = Manager();

void init() {
	//���� �׽�Ʈ Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);

	//���̽� �ø�
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	//���� ���� Ȱ��ȭ
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Ŀ�� �����
	glutSetCursor(GLUT_CURSOR_NONE);

	//��üȭ��
	glutFullScreen();

	//�Ŵ��� �ʱ�ȭ �� �ؽ�ó �ε�
	manager.init();
	manager.load_textures();
}

void reshape(int w, int h) {
	//����Ʈ ���� �� ���� ����, ī�޶� ����, ���콺 ��ġ �߽����� �̵�
	if (w == 0) h = 1;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float) w / (float) h, 0.1f, 100.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//�Ŵ������� ũ�� ������ ����
	manager.screen_size.x = w;
	manager.screen_size.y = h;

	glutWarpPointer(manager.screen_size.x / 2, manager.screen_size.y / 2);
}

void light() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//���� ����
	float position0[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	float ambient0[4] = {0.0625f, 0.125f, 0.25f, 1.0f};
	float diffuse0[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	float specular0[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0625f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.03125f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.03125f / 2);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//����
	light();

	//�÷��̾��� ��ġ�� �ٶ󺸴� ��Ҹ� ����
	glm::dvec3 cam_pos = manager.player->get_camera_pos();
	glm::dvec3 cam_center = manager.player->get_camera_center();

	//ī�޶� ���� ������ ���� �ٶ󺸰� ��
	gluLookAt(
		cam_pos.x, cam_pos.y, cam_pos.z,
		cam_center.x, cam_center.y, cam_center.z,
		0, 1, 0
	);

	//�Ŵ����� ��ü���� �׸�
	manager.draw();

	glDisable(GL_LIGHTING);

	//�Ŵ����� ��ü���� HUD �׸�
	manager.draw_hud();

	//���� ���۸�
	glutSwapBuffers();
}

void idle() {
	//���콺 �̵��� ���� �÷��̾��� ī�޶� ȸ��
	manager.player->camera_rotation[0] = -manager.diff_mouse.x * 0.0078125;
	manager.player->camera_rotation[1] = manager.diff_mouse.y * 0.0078125;
	manager.diff_mouse.x = 0;
	manager.diff_mouse.y = 0;

	//�Ŵ����� ��ü���� ������Ʈ
	manager.update();
	
	//��� �ٽ� �׸�
	glutPostRedisplay();
}

void keyboard_down(unsigned char key, int x, int y) {
	//����Ű�� ���� �÷��̾��� �̵� ����
	//������ �ش� ���� Ȱ��ȭ
	switch (key) {
		case 'W':
		case 'w':
			manager.player->movement_state[0] = true;
			break;
		case 'S':
		case 's':
			manager.player->movement_state[1] = true;
			break;
		case 'A':
		case 'a':
			manager.player->movement_state[2] = true;
			break;
		case 'D':
		case 'd':
			manager.player->movement_state[3] = true;
			break;
		case ' ':
			manager.player->movement_state[4] = true;
			break;
		case '\033':
			//ESC Ű�� ���α׷��� ��� ����
			exit(0);
	}
}

void keyboard_up(unsigned char key, int x, int y) {
	//����⿡ ���� �÷��̾��� �̵� ����
	//�������� �ش� ���� ��Ȱ��ȭ
	switch (key) {
		case 'W':
		case 'w':
			manager.player->movement_state[0] = false;
			break;
		case 'S':
		case 's':
			manager.player->movement_state[1] = false;
			break;
		case 'A':
		case 'a':
			manager.player->movement_state[2] = false;
			break;
		case 'D':
		case 'd':
			manager.player->movement_state[3] = false;
			break;
		case ' ':
			manager.player->movement_state[4] = false;
			break;
	}
}

void mouse(int button, int state, int w, int h) {
	switch (button) {
		case GLUT_LEFT_BUTTON:
			//���콺 ��ư�� ������ �÷��̾ �߻��ϴ� ����
			if (state == GLUT_DOWN) manager.player->shooting = true;
			else manager.player->shooting = false;
			break;
	}
}

void passive_motion(int w, int h) {
	//���콺 ��ġ�� ���� ���ϰ�, ���콺�� ȭ�� �� �߾����� �̵����� �þ߸� �������� ȸ��
	manager.diff_mouse.x = w - manager.last_mouse.x;
	manager.diff_mouse.y = h - manager.last_mouse.y;

	manager.last_mouse.x = manager.screen_size.x / 2;
	manager.last_mouse.y = manager.screen_size.y / 2;

	glutWarpPointer(manager.screen_size.x / 2, manager.screen_size.y / 2);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(manager.screen_size.x, manager.screen_size.y);
	glutCreateWindow("OpenGL Project");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard_down);
	glutKeyboardUpFunc(keyboard_up);
	glutMouseFunc(mouse);
	//���콺 ��ǰ� �нú� ����� �������� ����
	glutPassiveMotionFunc(passive_motion);
	glutMotionFunc(passive_motion);
	glutIdleFunc(idle);
	//���� ���� ���� �� �ʱ�ȭ
	init();
	glutMainLoop();
}