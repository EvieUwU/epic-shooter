#include "main.h"

int time_between_actions[6] = {0, 30, 25, 45, 2, 25};

/**
 * @brief Function for fade in-out effect
 * 
 * @param x X coord for the sigmoid function
 * @return float (between 0 and 1)
 */
float sigmoid(float x) {
	return 1 / (1 + exp(-x));
}

/**
 * @brief Runs enemy behaviour for a frame
 * 
 */
void enemy_behaviour() {
	for (int i = 0; i < num_enemies; i++) {
		enemy *el = &enemies[i];
		if (el->type == ENEMY_TYPE_BOSS) {
			boss_logic(el);
			continue;
		}
		enemy_every_frame(el);
		enemy_data *e = &el->data;
		if (e->since_last_action < time_between_actions[el->type] || 
			(el->type == ENEMY_TYPE_HAILER && e->since_last_action < el->hailer.cooldown)) {
			e->since_last_action++;
			continue;
		}
		enemy_actions(el);
	}
}

/**
 * @brief Runs enemy logic every frame
 * 
 * @param en The enemy to run logic for
 */
void enemy_every_frame(enemy *en) {
	enemy_data *e = &en->data;
	switch (en->type) {
		case ENEMY_TYPE_NONE:
			break;
		case ENEMY_TYPE_SCATTER:
			break;
		case ENEMY_TYPE_AIMER:
			if (en->aimer.going_up) {
				en->aimer.percentage -= 0.005;
			} else {
				en->aimer.percentage += 0.005;
			}
			float sig = 10 * en->aimer.percentage - 5;
			e->pos.y = e->h + sigmoid(sig) * (TOP_SCREEN_HEIGHT - e->h * 2);
			if (en->aimer.percentage <= 0.02) {
				en->aimer.going_up = false;
			} else if (en->aimer.percentage >= 0.98) {
				en->aimer.going_up = true;
			}
			break;
		case ENEMY_TYPE_MACHINE:
			if (en->machine.shooting) {
				en->machine.cooldown--;
				if (en->machine.cooldown <= 0) {
					en->machine.shooting = false;
					en->machine.cooldown = 90;
				}
			} else {
				en->machine.cooldown--;
				if (en->machine.cooldown <= 0) {
					en->machine.shooting = true;
					en->machine.cooldown = 45;
				}
				float ydiff = pl.pos.y - e->pos.y;
				if (ydiff < 2 && ydiff > -2) {
					e->pos.y = pl.pos.y;
				} else {
					e->pos.y += ydiff / 32;
				}
			}
			break;
		case ENEMY_TYPE_HAILER:
			en->hailer.cooldown -= 0.01;
			if (en->hailer.cooldown < 1) {
				en->hailer.cooldown = 1;
			}
			break;
		default:
			break;
	}
}

/**
 * @brief Runs enemy actions if they are due
 * 
 * @param en The enemy to run actions for
 */
void enemy_actions(enemy *en) {
	enemy_data *e = &en->data;
	switch (en->type) {
		case ENEMY_TYPE_NONE:
			break;
		case ENEMY_TYPE_SCATTER:
			switch (e->actions) {
				case 0:
					create_bullet(e->pos.x - e->w, e->pos.y + e->h / 2 - 1, en->type, -BULLET_SPEED, 0);
					e->actions++;
					e->since_last_action = 0;
					break;
				case 1:
					create_bullet(e->pos.x - e->w, e->pos.y + e->h / 2 - 1, en->type, -BULLET_SPEED, 0);
					e->actions++;
					e->since_last_action = 0;
					break;
				case 2:
					if (e->since_last_action <= 30) {
						en->scatter.newpos.x = (3 * TOP_SCREEN_WIDTH / 4) + rand() % (TOP_SCREEN_WIDTH / 4);
						en->scatter.newpos.y = rand() % (TOP_SCREEN_HEIGHT - e->h);
						en->scatter.oldpos.x = e->pos.x;
						en->scatter.oldpos.y = e->pos.y;
					} else if (e->since_last_action < 60) {
						e->pos.x = en->scatter.oldpos.x + ((1 - (float)(60 - e->since_last_action) / 30.0f)) * (en->scatter.newpos.x - en->scatter.oldpos.x);
						e->pos.y = en->scatter.oldpos.y + ((1 - (float)(60 - e->since_last_action) / 30.0f)) * (en->scatter.newpos.y - en->scatter.oldpos.y);
					} else {
						e->pos.x = en->scatter.newpos.x;
						e->pos.y = en->scatter.newpos.y;
						e->actions = 0;
						e->since_last_action = 0;
					}	
					e->since_last_action++;
					break;
				default:
					break;
			}
			break;
		case ENEMY_TYPE_AIMER:
			float m = 0;
			float len = 0;
			switch (e->actions) {
				case 0:
					// Fire bullet
					m = ((pl.pos.y + pl.h / 2) - (e->pos.y + e->h / 2 - 2)) / ((pl.pos.x + pl.w / 2) - (e->pos.x + e->w / 4));
					// Normalize vector, then multiply by 4
					len = sqrt(1 + m * m);

					create_bullet(e->pos.x + e->w / 4, e->pos.y + e->h / 2 - 2, en->type, (-1 / len) * BULLET_SPEED, (-m / len) * BULLET_SPEED);

					e->since_last_action = 0;
					break;
				default:
					break;
			}
			break;
		case ENEMY_TYPE_MACHINE:
			switch (e->actions) {
				case 0:
					// Fire bullet
					if (en->machine.shooting)
						create_bullet(e->pos.x - e->w / 4, e->pos.y + e->h / 2, en->type, -BULLET_SPEED, 0);
					e->since_last_action = 0;
					break;
				default:
					break;
			}
			break;
		case ENEMY_TYPE_HAILER:
			switch (e->actions) {
				case 0:
					// Fire bullet
					if (rand() % 2 <= 1) {
						create_bullet(rand() % (TOP_SCREEN_WIDTH - 60), 0, en->type, rand() % BULLET_SPEED, 5);
					} else {
						create_bullet(rand() % (TOP_SCREEN_WIDTH - 60), 0, en->type, -1 * (rand() % BULLET_SPEED), 5);
					}
					e->since_last_action = 0;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}