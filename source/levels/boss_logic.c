#include "../main.h"

point circle_offsets[5] = {
	{-0.5, -0.87},
	{-0.87, -0.5},
	{-1, 0},
	{-0.87, 0.5},
	{-0.5, 0.87}
};

void boss_logic(enemy *en) {
	en->data.since_last_action++;
	switch (en->boss.type) {
		case BOSS_TYPE_PROG:
			boss_prog_logic(en);
			break;
	}
}

bool boss_collision(bullet *cur, enemy *en) {
	switch (en->boss.type) {
		case BOSS_TYPE_PROG:
			point a = (point){en->data.pos.x, en->data.pos.y};
			point b = (point){en->data.pos.x + en->data.w, en->data.pos.y};
			point c = (point){en->data.pos.x + en->data.w, en->data.pos.y + en->data.h};
			point d = (point){en->data.pos.x, en->data.pos.y + en->data.h};
			if (inside_rect(cur->pos, a, b, c, d)) {
				en->data.health -= 10;
				score += 10;
				return true;
			}
			return false;
		default:
			return false;
	}
}

void boss_spawn(boss_type type, int x, int y, int w, int h, int health) {
	switch (type) {
		case BOSS_TYPE_PROG:
			create_enemy(ENEMY_TYPE_BOSS, (point){x, y}, w, h, health);
			enemies[num_enemies - 1].boss.type = type;
			enemies[num_enemies - 1].boss.rotation = 0;
			enemies[num_enemies - 1].boss.cooldown = 0;
			enemies[num_enemies - 1].boss.sprite = boss_sprites[0].image;
			enemies[num_enemies - 1].boss.prog.percentage = y / TOP_SCREEN_HEIGHT;
			enemies[num_enemies - 1].boss.prog.phase = 0;
			enemies[num_enemies - 1].boss.prog.going_up = rand() % 2 == 0;
			enemies[num_enemies - 1].boss.prog.shooting_cooldown = 60;
			enemies[num_enemies - 1].boss.prog.shots = 0;
			break;
	}
}

void boss_render(enemy *en) {
	switch (en->boss.type) {
		case BOSS_TYPE_PROG:
			printf("\x1b[1;1HBoss Position: %f, %f\x1b[K", en->data.pos.x, en->data.pos.y);
			printf("\x1b[2;1HBoss Rotation: %f\x1b[K", en->boss.rotation);
			printf("\x1b[3;1HBoss Scale: %f, %f\x1b[K", (float)en->data.w / (float)boss_sprites[0].image.tex->width, (float)en->data.h / (float)boss_sprites[0].image.tex->height);
			C2D_DrawImageAt(en->boss.sprite, en->data.pos.x, en->data.pos.y, 1, NULL,
							(float)en->data.w / (float)boss_sprites[0].image.tex->width, (float)en->data.h / (float)boss_sprites[0].image.tex->height);
			break;
		default:
			break;
	}
}

void boss_prog_logic(enemy *en) {
	if (en->boss.prog.phase == 0) {
		if (en->boss.prog.going_up) {
			en->boss.prog.percentage -= 0.005;
		} else {
			en->boss.prog.percentage += 0.005;
		}
		float sig = 10 * en->boss.prog.percentage - 5;
		en->data.pos.y = sigmoid(sig) * (TOP_SCREEN_HEIGHT - en->data.h);
		if (en->boss.prog.percentage <= 0.01) {
			en->boss.prog.going_up = false;
		} else if (en->boss.prog.percentage >= 0.99) {
			en->boss.prog.going_up = true;
			en->boss.prog.wait = true;
			en->boss.prog.phase = 1;
			en->boss.prog.shots = 0;
			en->boss.prog.shooting_cooldown = 30;
		}

		en->boss.prog.shooting_cooldown--;

		if (en->boss.prog.shots < 7 && en->boss.prog.shooting_cooldown <= 0) {
			create_bullet(en->data.pos.x, en->data.pos.y + en->data.h / 6, ENEMY_TYPE_BOSS, -BULLET_SPEED, 0);
			create_bullet(en->data.pos.x, en->data.pos.y + 5 * en->data.h / 6, ENEMY_TYPE_BOSS, -BULLET_SPEED, 0);
			en->boss.prog.shots++;
			en->boss.prog.shooting_cooldown = 5;
		} else if (en->boss.prog.shots >= 7) {
			en->boss.prog.shooting_cooldown = 10 + 60 * (float)en->data.health / en->data.max_health;
			en->boss.prog.shots = 0;
		}
	} else if (en->boss.prog.phase == 1) {
		if (!en->boss.prog.wait) {
			if (en->boss.prog.going_up) {
				en->boss.prog.percentage -= 0.005;
			} else {
				en->boss.prog.percentage += 0.005;
			}
			float sig = 10 * en->boss.prog.percentage - 5;
			en->data.pos.y = sigmoid(sig) * (TOP_SCREEN_HEIGHT - en->data.h);
		}
		if (en->boss.prog.percentage <= 0.01 || en->boss.prog.percentage >= 0.99) {
			en->boss.prog.wait = true;
		} else {
			en->boss.prog.wait = false;
		}

		en->boss.prog.shooting_cooldown--;

		if (en->boss.prog.wait && en->boss.prog.shooting_cooldown <= 0 && en->boss.prog.shots <= 3) {
			en->boss.prog.shooting_cooldown = 10;
			en->boss.prog.shots++;
			for (int i = 0; i < 5; i++) {
				create_bullet(en->data.pos.x, en->data.pos.y + en->data.h / 2, ENEMY_TYPE_BOSS, circle_offsets[i].x * BULLET_SPEED, circle_offsets[i].y * BULLET_SPEED);
			}
		} else if (en->boss.prog.shots > 3) {
			en->boss.prog.shots = 0;
			en->boss.prog.wait = false;
			if (en->boss.prog.percentage >= 0.99) {
				en->boss.prog.percentage = 0.99;
				en->boss.prog.going_up = true;
			}
			if (en->boss.prog.percentage <= 0.01) {
				en->boss.prog.phase = 0;
				en->boss.prog.going_up = false;
			}
		}
	}
}