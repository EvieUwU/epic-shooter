#include "main.h"
bool speed = false;

int SCATTER_HEALTH = 20;
int AIMER_HEALTH = 80;
int MACHINE_HEALTH = 120;
int HAILER_HEALTH = 100;

int PROG_HEALTH = 500;

void level_spawner(int current_lv, level levels[]) {
	if (next_wave >= levels[current_lv].num_waves) {
		if (num_enemies <= 0) {
			setState(STATE_LEVEL_WIN);
			won_levels[current_lv] = true;
		}
		return;
	} else if (next_wave == 0 || 
		frame_num - last_wave_spawn >= seconds(levels[current_lv].waves[next_wave - 1].duration)) {
		speed = false;
		wave current = levels[current_lv].waves[next_wave];
		if (current.flags & WAVE_TYPE_ENEMY) {
			for (int i = 0; i < current.enemy_wave.num_enemies; i++) {
				enemy_type type = current.enemy_wave.enemies[i];
				switch (type) {
					case ENEMY_TYPE_NONE:
					case ENEMY_TYPE_BOSS:
						continue;
					case ENEMY_TYPE_SCATTER:
						create_enemy(ENEMY_TYPE_SCATTER, (point){(3 * TOP_SCREEN_WIDTH / 4) + rand() % (TOP_SCREEN_WIDTH / 4), rand() % (TOP_SCREEN_HEIGHT - 20)}, 20, 20, SCATTER_HEALTH);
						break;
					case ENEMY_TYPE_AIMER:
						create_enemy(ENEMY_TYPE_AIMER, (point){TOP_SCREEN_WIDTH - 40 - (rand() % 20), rand() % (TOP_SCREEN_HEIGHT - 30)}, 15, 15, AIMER_HEALTH);
						break;
					case ENEMY_TYPE_MACHINE:
						create_enemy(ENEMY_TYPE_MACHINE, (point){TOP_SCREEN_WIDTH - 60 - (rand() % 40), rand() % (TOP_SCREEN_HEIGHT - 25)}, 25, 25, MACHINE_HEALTH);
						break;
					case ENEMY_TYPE_HAILER:
						create_enemy(ENEMY_TYPE_HAILER, (point){TOP_SCREEN_WIDTH - 30 - (rand() % 20), rand() % (TOP_SCREEN_HEIGHT - 40)}, 20, 20, HAILER_HEALTH);
						break;
					default:
						break;
				}
			}
		}
		if (current.flags & WAVE_TYPE_BOSS) {
			for (int i = 0; i < current.boss_wave.num_bosses; i++) {
				boss_type type = current.boss_wave.bosses[i];
				switch (type) {
					case BOSS_TYPE_PROG:
						boss_spawn(type, TOP_SCREEN_WIDTH - 80, TOP_SCREEN_HEIGHT / 2, 64, 64, PROG_HEALTH);
						break;
					default:
						break;
				}
			}
		}
		last_wave_spawn = frame_num;
		next_wave++;
	}
	if (num_enemies <= 0) {
		if (frame_num - last_wave_spawn >= seconds(levels[current_lv].waves[next_wave - 1].duration / 2.0f)) {
			set_bonus_active("Speedy", false);
		}
		if (frame_num - last_wave_spawn >= seconds(3 * levels[current_lv].waves[next_wave - 1].duration / 4.0f)) {
			set_bonus_active("Fast", false);
		}
		pl.health += 0.2;
		if (pl.health >= pl.max_health) {
			pl.health = pl.max_health;
			speed = true;
			last_wave_spawn -= 2;
			if (frame_num - last_wave_spawn >= seconds(levels[current_lv].waves[next_wave - 1].duration)) {
				last_wave_spawn = frame_num - seconds(levels[current_lv].waves[next_wave - 1].duration);
				speed = false;
			}
		}
	}
}

void level_bg(int level) {
	// Use since_state_change as scroll
	background(colour_blue, colour_blue, colour_dark_blue, colour_dark_blue);
	int x = 0 - since_state_change;
	int w = TOP_SCREEN_WIDTH * 3;
	int h = 75;
	while (x + w < 0) {
		x += w;
	}
	while (x < TOP_SCREEN_WIDTH) {
		// Draw bottom section
		C2D_DrawTriangle(x, TOP_SCREEN_HEIGHT, colour_cyan, x + w / 4, TOP_SCREEN_HEIGHT - h, colour_cyan, x + w / 2, TOP_SCREEN_HEIGHT - h / 2, colour_dark_cyan, 0);
		C2D_DrawTriangle(x, TOP_SCREEN_HEIGHT, colour_cyan, x + w / 2, TOP_SCREEN_HEIGHT - h / 2, colour_dark_cyan, x + w / 2, TOP_SCREEN_HEIGHT, colour_dark_cyan, 0);
		C2D_DrawTriangle(x + w / 2, TOP_SCREEN_HEIGHT - h / 2, colour_dark_cyan, x + 7 * w / 8, TOP_SCREEN_HEIGHT - 7 * h / 8, colour_cyan, x + w / 2, TOP_SCREEN_HEIGHT, colour_dark_cyan, 0);
		C2D_DrawTriangle(x + 3 * w / 8, TOP_SCREEN_HEIGHT, colour_dark_cyan, x + 7 * w / 8, TOP_SCREEN_HEIGHT - 7 * h / 8, colour_cyan, x + w, TOP_SCREEN_HEIGHT, colour_cyan, 0);

		x += w;
	}
	x = 0 - since_state_change * 2;
	w = TOP_SCREEN_WIDTH / 2;
	h = 50;
	while (x + w < 0) {
		x += w;
	}
	while (x < TOP_SCREEN_WIDTH) {
		// Draw top section

		C2D_DrawRectangle(x, 0, 0, w, h / 8, colour_dark_cyan, colour_dark_cyan, colour_dark_cyan, colour_dark_cyan);

		C2D_DrawTriangle(x, h / 8, colour_dark_cyan, x + w / 4, h, colour_cyan, x + w / 2, 5 * h / 8, colour_cyan, 0);
		C2D_DrawTriangle(x, h / 8, colour_dark_cyan, x + w / 2, 5 * h / 8, colour_cyan, x + w / 2, h / 8, colour_dark_cyan, 0);
		C2D_DrawTriangle(x + w / 2, 5 * h / 8, colour_cyan, x + 7 * w / 8, h, colour_cyan, x + w / 2, h / 8, colour_dark_cyan, 0);
		C2D_DrawTriangle(x + w / 2, h / 8, colour_dark_cyan, x + 7 * w / 8, h, colour_cyan, x + w, h / 8, colour_dark_cyan, 0);

		x += w;
	}
}