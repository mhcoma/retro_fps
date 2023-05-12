#include "Map.h"

#include "Enemy.h"

bool Map::load() {
	FILE* fp;
	fopen_s(&fp, "map.txt", "r");

	if (!fp) return false;

	int temp;
	int row_index = 0;
	int column_index = 0;
	while (fscanf(fp, "%d", &temp) != EOF) {
		this->map[map_size - row_index - 1][column_index] = temp;
		if (temp == BlockType::bt_enemy) {
			Enemy* enemy = new Enemy();
			enemy->pos = glm::dvec3(
				-this->map_coord_to_actor_pos(row_index) - 3,
				1,
				this->map_coord_to_actor_pos(column_index) + 1
			);
			enemy->map = this;
			enemy->manager = this->manager;

			this->manager->actors.push_back(enemy);
		}
		column_index++;
		if (column_index >= map_size) {
			column_index = 0;
			row_index++;
		}
		if (row_index >= map_size) {
			break;
		}
	}
	fclose(fp);

	return true;
}

void Map::draw() {
	glEnable(GL_TEXTURE_2D);


	for (int i = 0; i < map_size; i++) {
		for (int j = 0; j < map_size; j++) {
			glPushMatrix();
			glTranslatef(i * 2 - (map_size), 0, j * 2 - (map_size));
			switch (this->map[i][j]) {
				case BlockType::bt_underground:
					this->draw_ground(-4, Texture::t_underground);
					this->draw_ceil();
					break;
				case BlockType::bt_ground:
					this->draw_ground(0, Texture::t_ground);
					this->draw_ceil();
					this->draw_underground_wall();
					break;
				case BlockType::bt_enemy:
				case BlockType::bt_half:
					this->draw_wall(1);
					this->draw_ground(1, Texture::t_underground);

					this->draw_ceil();
					this->draw_underground_wall();
					break;
				case BlockType::bt_wall:
					this->draw_wall(4);
					this->draw_underground_wall();
					break;
			}
			glPopMatrix();
		}
	}
	glDisable(GL_TEXTURE_2D);
}

void Map::draw_underground_wall() {
	glBindTexture(GL_TEXTURE_2D, this->manager->textures_indices[Texture::t_underground]);

	glBegin(GL_QUADS);

	glNormal3f(0, 0, -1);
	glTexCoord2f(2, 4);
	glVertex3f(-1, -4, -1);
	glTexCoord2f(2, 0);
	glVertex3f(-1, 0, -1);
	glTexCoord2f(0, 0);
	glVertex3f(1, 0, -1);
	glTexCoord2f(0, 4);
	glVertex3f(1, -4, -1);

	glNormal3f(0, 0, 1);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 0, 1);
	glTexCoord2f(0, 4);
	glVertex3f(-1, -4, 1);
	glTexCoord2f(2, 4);
	glVertex3f(1, -4, 1);
	glTexCoord2f(2, 0);
	glVertex3f(1, 0, 1);

	glNormal3f(1, 0, 0);
	glTexCoord2f(2, 4);
	glVertex3f(1, -4, -1);
	glTexCoord2f(2, 0);
	glVertex3f(1, 0, -1);
	glTexCoord2f(0, 0);
	glVertex3f(1, 0, 1);
	glTexCoord2f(0, 4);
	glVertex3f(1, -4, 1);

	glNormal3f(-1, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 0, -1);
	glTexCoord2f(0, 4);
	glVertex3f(-1, -4, -1);
	glTexCoord2f(2, 4);
	glVertex3f(-1, -4, 1);
	glTexCoord2f(2, 0);
	glVertex3f(-1, 0, 1);

	glEnd();
}


void Map::draw_ground(int height, int texture) {
	glBindTexture(GL_TEXTURE_2D, this->manager->textures_indices[texture]);
	glBegin(GL_QUADS);

	glNormal3f(0, 1, 0);
	glTexCoord2f(2, 2);
	glVertex3f(-1, height, -1);
	glTexCoord2f(2, 0);
	glVertex3f(-1, height, 1);
	glTexCoord2f(0, 0);
	glVertex3f(1, height, 1);
	glTexCoord2f(0, 2);
	glVertex3f(1, height, -1);


	glEnd();
}

void Map::draw_ceil() {
	glBindTexture(GL_TEXTURE_2D, this->manager->textures_indices[Texture::t_ceil]);

	glBegin(GL_QUADS);

	glNormal3f(0, -1, 0);
	glTexCoord2f(2, 2);
	glVertex3f(1, 4, 1);
	glTexCoord2f(2, 0);
	glVertex3f(-1, 4, 1);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 4, -1);
	glTexCoord2f(0, 2);
	glVertex3f(1, 4, -1);

	glEnd();
}

void Map::draw_wall(int height) {
	glBindTexture(GL_TEXTURE_2D, this->manager->textures_indices[Texture::t_wall]);

	glBegin(GL_QUADS);

	glNormal3f(0, 0, -1);
	glTexCoord2f(2, height);
	glVertex3f(-1, 0, -1);
	glTexCoord2f(2, 0);
	glVertex3f(-1, height, -1);
	glTexCoord2f(0, 0);
	glVertex3f(1, height, -1);
	glTexCoord2f(0, height);
	glVertex3f(1, 0, -1);

	glNormal3f(0, 0, 1);
	glTexCoord2f(0, 0);
	glVertex3f(-1, height, 1);
	glTexCoord2f(0, height);
	glVertex3f(-1, 0, 1);
	glTexCoord2f(2, height);
	glVertex3f(1, 0, 1);
	glTexCoord2f(2, 0);
	glVertex3f(1, height, 1);

	glNormal3f(1, 0, 0);
	glTexCoord2f(2, height);
	glVertex3f(1, 0, -1);
	glTexCoord2f(2, 0);
	glVertex3f(1, height, -1);
	glTexCoord2f(0, 0);
	glVertex3f(1, height, 1);
	glTexCoord2f(0, height);
	glVertex3f(1, 0, 1);

	glNormal3f(-1, 0, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, height, -1);
	glTexCoord2f(0, height);
	glVertex3f(-1, 0, -1);
	glTexCoord2f(2, height);
	glVertex3f(-1, 0, 1);
	glTexCoord2f(2, 0);
	glVertex3f(-1, height, 1);

	glEnd();
}