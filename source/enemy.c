#include "main.h"

// Setup enemies
enemy enemies[MAX_ENEMIES];
int num_enemies = 0;

/**
 * @brief Create a enemy object
 * 
 * @param type Type of enemy
 * @param pos Enemy starting position
 * @param w Enemy width
 * @param h Enemy height
 * @param health Enemy health
 */
void create_enemy(enemy_type type, point pos, int w, int h, int health) {
	enemy *e = &enemies[num_enemies];
	e->type = type;
	e->data.w = w;
	e->data.h = h;
	e->data.pos.x = pos.x;
	e->data.pos.y = pos.y;
	e->data.health = health;
	e->data.max_health = health;
	e->data.since_last_action = 0;
	e->data.actions = 0;
	switch (type) {
		case ENEMY_TYPE_NONE:
		case ENEMY_TYPE_BOSS:
			break;
		case ENEMY_TYPE_SCATTER:
			e->scatter.newpos = (point){0, 0};
			e->scatter.oldpos = (point){0, 0};
			break;
		case ENEMY_TYPE_AIMER:
			if (rand() % 1 > 0.5) {
				e->aimer.going_up = true;
			} else {
				e->aimer.going_up = false;
			}
			e->aimer.percentage = rand() % 100 / 100.0f;
			break;
		case ENEMY_TYPE_MACHINE:
			e->machine.shooting = false;
			e->machine.cooldown = 30;
			break;
		case ENEMY_TYPE_HAILER:
			e->hailer.cooldown = 30;
			break;
		default:
			break;
	}
	num_enemies++;
}

/**
 * @brief Removes an enemy by index
 * 
 * @param idx Index to remove
 */
void remove_enemy(int idx) {
	if (idx >= num_enemies) {
		return;
	}
	for (int i = idx; i < num_enemies - 1; i++) {
		enemies[i] = enemies[i + 1];
	}
	num_enemies--;
}