#include <iostream>
#include <vector>

#include "glut.h"

#include "Coord.h"
#include "Manager.h"
#include "Map.h"
#include "Player.h"

Manager manager = Manager();

void init() {
	//깊이 테스트 활성화
	glEnable(GL_DEPTH_TEST);

	//페이스 컬링
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	//알파 블렌딩 활성화
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//커서 지우기
	glutSetCursor(GLUT_CURSOR_NONE);

	//전체화면
	glutFullScreen();

	//매니저 초기화 및 텍스처 로드
	manager.init();
	manager.load_textures();
}

void reshape(int w, int h) {
	//뷰포트 설정 및 시점 설정, 카메라 설정, 마우스 위치 중심으로 이동
	if (w == 0) h = 1;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float) w / (float) h, 0.1f, 100.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//매니저에게 크기 정보를 전달
	manager.screen_size.x = w;
	manager.screen_size.y = h;

	glutWarpPointer(manager.screen_size.x / 2, manager.screen_size.y / 2);
}

void light() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//광원 설정
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

	//광원
	light();

	//플레이어의 위치와 바라보는 장소를 구함
	glm::dvec3 cam_pos = manager.player->get_camera_pos();
	glm::dvec3 cam_center = manager.player->get_camera_center();

	//카메라가 구한 정보를 토대로 바라보게 함
	gluLookAt(
		cam_pos.x, cam_pos.y, cam_pos.z,
		cam_center.x, cam_center.y, cam_center.z,
		0, 1, 0
	);

	//매니저가 객체들을 그림
	manager.draw();

	glDisable(GL_LIGHTING);

	//매니저가 객체들의 HUD 그림
	manager.draw_hud();

	//더블 버퍼링
	glutSwapBuffers();
}

void idle() {
	//마우스 이동에 따라 플레이어의 카메라 회전
	manager.player->camera_rotation[0] = -manager.diff_mouse.x * 0.0078125;
	manager.player->camera_rotation[1] = manager.diff_mouse.y * 0.0078125;
	manager.diff_mouse.x = 0;
	manager.diff_mouse.y = 0;

	//매니저가 객체들을 업데이트
	manager.update();
	
	//즉시 다시 그림
	glutPostRedisplay();
}

void keyboard_down(unsigned char key, int x, int y) {
	//방향키에 따라 플레이어의 이동 설정
	//눌리면 해당 방향 활성화
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
			//ESC 키는 프로그램을 즉시 종료
			exit(0);
	}
}

void keyboard_up(unsigned char key, int x, int y) {
	//방향기에 따라 플레이어의 이동 설정
	//떼어지면 해당 방향 비활성화
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
			//마우스 버튼이 눌리면 플레이어가 발사하는 상태
			if (state == GLUT_DOWN) manager.player->shooting = true;
			else manager.player->shooting = false;
			break;
	}
}

void passive_motion(int w, int h) {
	//마우스 위치의 차를 구하고, 마우스를 화면 정 중앙으로 이동시켜 시야를 무한으로 회전
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
	//마우스 모션과 패시브 모션을 구분하지 않음
	glutPassiveMotionFunc(passive_motion);
	glutMotionFunc(passive_motion);
	glutIdleFunc(idle);
	//메인 루프 시작 전 초기화
	init();
	glutMainLoop();
}