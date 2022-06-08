#include "main.h"

/**
 * @brief Reads the state of the circle pad, and takes into account the dead zone.
 * 
 * @param dead_zone Values less than this will be considered as 0.
 * @return circlePosition 
 */
circlePosition readCStick(int dead_zone) {
	circlePosition pos;

	hidCircleRead(&pos);

	if (abs(pos.dx) < dead_zone) {
		pos.dx = 0;
	}
	if (abs(pos.dy) < dead_zone) {
		pos.dy = 0;
	}
	return pos;
}

float sign (point p1, point p2, point p3) {
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

/**
 * @brief Converts seconds into frames
 * 
 * @param secs Number of seconds
 * @return int
 */
int seconds(float secs) {
	return FRAMERATE * secs;
}

/**
 * @brief Create a bullet object
 * 
 * @param x Bullet X
 * @param y Bullet Y
 * @param from The type of enemy this came from
 * @param vel_x Starting X velocity
 * @param vel_y Starting Y velocity
 */
void create_bullet(float x, float y, enemy_type from, float vel_x, float vel_y) {
	if (num_bullets >= MAX_BULLETS) {
		return;
	}
	bullets[num_bullets].from = from;
	bullets[num_bullets].pos.x = x;
	bullets[num_bullets].pos.y = y;
	bullets[num_bullets].vel.x = vel_x;
	bullets[num_bullets].vel.y = vel_y;
	num_bullets++;
}

/**
 * @brief Deletes a bullet from the array.
 * 
 * @param idx The index to remove
 */
void delete_bullet(int idx) {
	if (idx >= num_bullets) {
		return;
	}
	for (int i = idx; i < num_bullets; i++) {
		bullets[i] = bullets[i + 1];
	}
	num_bullets--;
}

/**
 * @brief Loads the sprites from the sprite sheet
 * 
 */
void init_sprites() {
	sprite_sheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!sprite_sheet) {
		printf("Failed to load spritesheet\n");
		svcBreak(USERBREAK_PANIC);
	}
}

void init_level_text(C2D_Font font, C2D_TextBuf buf) {
	C2D_TextFontParse(&level_text[0], font, buf, "Introduction");
	C2D_TextFontParse(&level_text[1], font, buf, "The Beginning");
	C2D_TextFontParse(&level_text[2], font, buf, "Progging Land");
	C2D_TextFontParse(&level_text[3], font, buf, "The Intense Progger Arrives!");
	C2D_TextFontParse(&level_text[4], font, buf, "The End?");

	for (int i = 0; i < 5; i++) {
		C2D_TextOptimize(&level_text[i]);
	}

	C2D_TextFontParse(&unlocked_text, font, buf, "Debug Mode Active");
	C2D_TextOptimize(&unlocked_text);
}

void delay() {
	frame_num++;
	now_time = osGetTime();
	u64 frame_time = now_time - last_time;
	last_time = now_time;
	if (frame_time < MS_PER_FRAME) {
		svcSleepThread(MS_PER_FRAME - frame_time);
	}
}

bool inside_tri(point p, point tri_1, point tri_2, point tri_3) {
	float d1 = sign(p, tri_1, tri_2);
	float d2 = sign(p, tri_2, tri_3);
	float d3 = sign(p, tri_3, tri_1);

	bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
	return !(has_neg && has_pos);
}

bool inside_rect(point p, point rect_1, point rect_2, point rect_3, point rect_4) {
	return inside_tri(p, rect_1, rect_2, rect_3) || inside_tri(p, rect_1, rect_3, rect_4);
}

circlePosition pos;

C3D_RenderTarget *top;
C3D_RenderTarget *bottom;

// Setup player bullets
int num_bullets = 0;
bullet bullets[MAX_BULLETS];

int last_bullet = 0;
int frame_num = 0;

// Setup player
player pl;

int time_since_last_enemy_spawn = 0;

bool paused = false;

int current_level = 1;

game_state state = STATE_MAP;
int since_state_change = 0;

u64 now_time;
u64 last_time;

C2D_SpriteSheet sprite_sheet;
C2D_Sprite sprites[MAX_SPRITES / 2];
C2D_Sprite boss_sprites[MAX_SPRITES / 2];
size_t num_sprites = MAX_SPRITES / 2;

int num_levels = 5;
C2D_Text level_text[5];
level levels[5];

int last_wave_spawn = 0;
int next_wave = 0;

int score = 0;
int final_score = 0;

C2D_Font font;

C2D_Text unlocked_text;

float since_last_bonus = 0;

int num_bonuses = 6;
bonus bonuses[6] = {
	{
		.name = "No Damage Taken",
		.value = 250,
		.active = true,
	},
	{
		.name = "First Try",
		.value = 500,
		.active = true,
	},
	{
		.name = "Speedy",
		.value = 100,
		.active = true,
	},
	{
		.name = "Fast",
		.value = 50,
		.active = true,
	},
	{
		.name = "Cheapskate",
		.value = 250,
		.active = true,
	},
	{
		.name = "Triggerhappy",
		.value = 50,
		.active = false,
	},
};

void spriteInit() {
	size_t numImages = C2D_SpriteSheetCount(sprite_sheet);
	for (size_t i = 0; i < numImages; i++) {
		C2D_SpriteFromSheet(&boss_sprites[i], sprite_sheet, i);
		C2D_SpriteSetCenter(&boss_sprites[i], 0.5f, 0.5f);
		C2D_SpriteSetPos(&boss_sprites[i], 0, 0);
		C2D_SpriteSetRotation(&boss_sprites[i], 0);
	}
}

inline void setState(game_state new_state) {
	state = new_state;
	since_state_change = 0;
}

inline float dist(point a, point b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

inline float dist_sq(point a, point b) {
	return pow(a.x - b.x, 2) + pow(a.y - b.y, 2);
}

int main(int argc, char* argv[]) {
	srand(time(NULL));
	srvInit(); // Init services
	aptInit(); // Init applets
	hidInit(); // Init input
	romfsInit(); // Init romfs
	gfxInitDefault(); // Init graphics
	gfxSet3D(false); // Set 3D off

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	levelsInit(levels);

	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	sprite_sheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!sprite_sheet) svcBreak(USERBREAK_PANIC);

	spriteInit();

	pl.w = 20;
	pl.h = 20;
	pl.pos = (point){TOP_SCREEN_WIDTH / 4, TOP_SCREEN_HEIGHT / 2 - pl.h / 2};
	pl.max_health = 100;
	pl.health = 100;
	pl.last_regen = 0;
	pl.speed = 0.025;

	font = C2D_FontLoadSystem(CFG_REGION_EUR);
	C2D_TextBuf g_staticBuf = C2D_TextBufNew(4096);
	C2D_Text paused_text;
	C2D_TextFontParse(&paused_text, font, g_staticBuf, "Paused.");
	C2D_TextOptimize(&paused_text);

	C2D_Text dead_text;
	C2D_TextFontParse(&dead_text, font, g_staticBuf, "You died!");
	C2D_TextOptimize(&dead_text);

	C2D_Text win_text;
	C2D_TextFontParse(&win_text, font, g_staticBuf, "You win!");
	C2D_TextOptimize(&win_text);

	C2D_Text time_lose_text;
	C2D_TextFontParse(&time_lose_text, font, g_staticBuf, "Timeout...");
	C2D_TextOptimize(&time_lose_text);

	init_sprites();
	init_level_text(font, g_staticBuf);

	bool unlocked = false;

	bonus valid_bonuses[num_bonuses];
	int num_valid_bonuses = 0;

	int bullets_fired = 0;

	// Main loop
	while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		if ((kHeld & KEY_L) && (kHeld & KEY_R)) { // L + R to quit
			break; // break in order to return to hbmenu
		}

		if (kDown & KEY_START) {
			paused = !paused;
		}

		if (paused) {
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, colour_clear);
			C2D_SceneBegin(top);

			C2D_DrawText(&paused_text, 0, TOP_SCREEN_WIDTH / 2 - paused_text.width / 2, TOP_SCREEN_HEIGHT / 2, 0.0f, 1, 1);

			C3D_FrameEnd(0);
			delay();
			continue;
		}

		if (state == STATE_DEAD) {
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, colour_clear);
			C2D_SceneBegin(top);

			C2D_DrawText(&dead_text, C2D_AtBaseline, TOP_SCREEN_WIDTH / 2 - dead_text.width / 2, TOP_SCREEN_HEIGHT / 2, 0.0f, 1, 1);

			if (since_state_change > seconds(2.5f)) {
				while (num_enemies > 0) {
					remove_enemy(0);
				}
				while (num_bullets > 0) {
					delete_bullet(0);
				}
				setState(STATE_MAP);
			}
		} else if (state == STATE_LOSE_TIME) {
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, colour_clear);
			C2D_SceneBegin(top);

			C2D_DrawText(&time_lose_text, C2D_AtBaseline, TOP_SCREEN_WIDTH / 2 - time_lose_text.width / 2, TOP_SCREEN_HEIGHT / 2, 0.0f, 1, 1);

			if (since_state_change > seconds(2.5f)) {
				while (num_enemies > 0) {
					remove_enemy(0);
				}
				while (num_bullets > 0) {
					delete_bullet(0);
				}
				score = 0;
				setState(STATE_MAP);
			}
		} else if (state == STATE_MAP) {
			pos = readCStick(CSTICK_DEAD_ZONE);
			if (since_map_change > seconds(1)) {
				if (kDown & KEY_CPAD_RIGHT) {
					selected_map++;
					if (selected_map >= num_levels) {
						selected_map--;
					} else {
						selected_map_changed = true;
						last_selected_map = selected_map - 1;
						since_map_change = 0;
					}
				}
				if (kDown & KEY_CPAD_LEFT) {
					selected_map--;
					if (selected_map < 0) {
						selected_map++;
					} else {
						selected_map_changed = true;
						last_selected_map = selected_map + 1;
						since_map_change = 0;
					}
				}
			}
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, colour_clear);
			C2D_SceneBegin(top);
			draw_map(unlocked);
			if (kHeld & KEY_L && kHeld & KEY_X && kHeld & KEY_Y) {
				unlocked = true;
			} else if (kDown & KEY_A) { // Press A to select a level
				if (unlocked || 
					((selected_map == 0 || won_levels[selected_map - 1]) && !won_levels[selected_map])) {
					setState(STATE_LEVEL);
					current_level = selected_map + 1;
					last_wave_spawn = 0;
					next_wave = 0;
				}
			}
			since_map_change++;
		} else if (state == STATE_LEVEL) {

			// Read C Stick
			pos = readCStick(CSTICK_DEAD_ZONE);

			// Invert y axis
			pos.dy *= -1;

			pl.pos.x += ((float)pos.dx * pl.speed);
			pl.pos.y += ((float)pos.dy * pl.speed);

			// Clamp player to screen
			if (pl.pos.x < 0) {
				pl.pos.x = 0;
			} else if (pl.pos.x + pl.w > 3 * TOP_SCREEN_WIDTH / 4) {
				pl.pos.x = 3 * TOP_SCREEN_WIDTH / 4 - pl.w;
			}

			if (pl.pos.y < 0) {
				pl.pos.y = 0;
			} else if (pl.pos.y + pl.h > TOP_SCREEN_HEIGHT) {
				pl.pos.y = TOP_SCREEN_HEIGHT - pl.h;
			}

			// Shoot bullet if A is pressed
			if (kHeld & KEY_A) {
				// Only shoot once per 5 frames (6 per second)
				if (frame_num - last_bullet > 5) {
					create_bullet(pl.pos.x + pl.w - 4, pl.pos.y + pl.h / 2 - 1, ENEMY_TYPE_NONE, BULLET_SPEED, 0);
					last_bullet = frame_num;
					bullets_fired++;
				}
			}

			level_spawner(current_level - 1, levels);

			if (unlocked)
				pl.health = pl.max_health;

			enemy_behaviour();

			for (int i = 0; i < num_bullets; i++) {
				bullet *cur = &bullets[i];
				cur->pos.x += cur->vel.x;
				cur->pos.y += cur->vel.y;

				// Check for collisions with enemies
				bool removed = false;
				if (cur->from == ENEMY_TYPE_NONE) {
					for (int j = 0; j < num_enemies; j++) {
						enemy *cur_enemy = &enemies[j];
						enemy_data *data = &cur_enemy->data;
						if (dist_sq(cur->pos, cur_enemy->data.pos) > pow(cur_enemy->data.w, 2)) {
							continue;
						}
						switch (cur_enemy->type) {
							case ENEMY_TYPE_SCATTER:
								// Arrow type enemies
								point v1 = (point){data->pos.x - data->w, data->pos.y + data->h / 2};
								point v2 = (point){data->pos.x, data->pos.y};
								point v3 = (point){data->pos.x - data->w / 4, data->pos.y + data->h / 2};

								if (inside_tri(cur->pos, v1, v2, v3)) {
									data->health -= 10;
									score += 10;
									if (data->health <= 0) {
										remove_enemy(j);
									}
									delete_bullet(i);
									removed = true;
									break;
								}

								v2 = (point){data->pos.x, data->pos.y + data->h};

								if (inside_tri(cur->pos, v1, v2, v3)) {
									data->health -= 10;
									score += 10;
									if (data->health <= 0) {
										remove_enemy(j);
									}
									delete_bullet(i);
									removed = true;
								}
								break;
							case ENEMY_TYPE_HAILER:
							case ENEMY_TYPE_AIMER:
								// Circle type enemies
								float dx = cur->pos.x - data->pos.x;
								float dy = cur->pos.y - data->pos.y;
								float dist = sqrt(dx * dx + dy * dy);
								if (dist <= data->w) {
									data->health -= 10;
									score += 10;
									if (data->health <= 0) {
										remove_enemy(j);
									}
									delete_bullet(i);
									removed = true;
								}
								break;
							case ENEMY_TYPE_MACHINE:
								// Rectangle type enemies
								point a = (point){data->pos.x, data->pos.y};
								point b = (point){data->pos.x + data->w, data->pos.y};
								point c = (point){data->pos.x + data->w, data->pos.y + data->h};
								point d = (point){data->pos.x, data->pos.y + data->h};
								if (inside_rect(cur->pos, a, b, c, d)) {
									data->health -= 10;
									score += 10;
									if (data->health <= 0) {
										remove_enemy(j);
									}
									delete_bullet(i);
									removed = true;
								}
								break;
							case ENEMY_TYPE_BOSS:
								if (boss_collision(cur, cur_enemy)) {
									delete_bullet(i);
									if (cur_enemy->data.health <= 0) {
										remove_enemy(j);
									}
								}
								break;
							case ENEMY_TYPE_NONE:
							default:
								break;
						}
					}
				} else {
					point v1 = (point){pl.pos.x + pl.w, pl.pos.y + pl.h / 2};
					point v2 = (point){pl.pos.x, pl.pos.y};
					point v3 = (point){pl.pos.x + pl.w / 4, pl.pos.y + pl.h / 2};

					if (inside_tri(cur->pos, v1, v2, v3)) {
						pl.health -= 10;
						set_bonus_active("No Damage Taken", false);
						if (pl.health <= 0) {
							setState(STATE_DEAD);
							pl.health = pl.max_health;
						}
						delete_bullet(i);
						removed = true;
					} else {
						v2 = (point){pl.pos.x, pl.pos.y + pl.h};

						if (inside_tri(cur->pos, v1, v2, v3)) {
							pl.health -= 10;
							set_bonus_active("No Damage Taken", false);
							if (pl.health <= 0) {
								setState(STATE_DEAD);
								pl.health = pl.max_health;
							}
							delete_bullet(i);
							removed = true;
						}
					}
				}
				if (!removed) {	
					if (cur->pos.x < 0 || cur->pos.x > TOP_SCREEN_WIDTH || cur->pos.y < 0 || cur->pos.y > TOP_SCREEN_HEIGHT) {
						delete_bullet(i);
					}
				} else {
					i--;
				}
			}

			// Start of draw routine
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, colour_clear);
			C2D_SceneBegin(top);

			level_bg(1);

			// Draw player
			C2D_DrawTriangle(	pl.pos.x + pl.w, pl.pos.y + pl.h / 2, colour_pl_front, 
								pl.pos.x, pl.pos.y, colour_pl_back,
								pl.pos.x + pl.w / 4, pl.pos.y + pl.h / 2, colour_pl_mid, 0);
			C2D_DrawTriangle(	pl.pos.x + pl.w / 4, pl.pos.y + pl.h / 2, colour_pl_mid,
								pl.pos.x, pl.pos.y + pl.h, colour_pl_back,
								pl.pos.x + pl.w, pl.pos.y + pl.h / 2, colour_pl_front, 0);

			// Draw health
			C2D_DrawRectSolid(5, 5, 0, pl.max_health / 2, 15, colour_white);
			C2D_DrawRectSolid(7, 7, 0, (pl.max_health / 2 - 4) * (pl.health / pl.max_health), 11, colour_red);

			// Draw boss health
			int y_offset = 0;
			for (int i = 0; i < num_enemies; i++) {
				if (enemies[i].type != ENEMY_TYPE_BOSS) continue;

				C2D_DrawRectSolid(TOP_SCREEN_WIDTH - 4 - 128, TOP_SCREEN_HEIGHT - 12 - y_offset, 0, 128, 8, colour_white);
				C2D_DrawRectSolid(TOP_SCREEN_WIDTH - 2 - 128, TOP_SCREEN_HEIGHT - 10 - y_offset, 0, 124.0f * ((float)enemies[i].data.health / enemies[i].data.max_health), 4, colour_red);
				y_offset += 12;
			}

			// Draw enemies
			for (int i = 0; i < num_enemies; i++) {
				enemy cur = enemies[i];
				enemy_data en = cur.data;
				switch (cur.type) {
					case ENEMY_TYPE_SCATTER:
						C2D_DrawTriangle(	en.pos.x - en.w, en.pos.y + en.h / 2, colour_scatter_front, 
											en.pos.x, en.pos.y, colour_scatter_back,
											en.pos.x - en.w / 4, en.pos.y + en.h / 2, colour_scatter_mid, 0);
						C2D_DrawTriangle(	en.pos.x - en.w / 4, en.pos.y + en.h / 2, colour_scatter_mid,
											en.pos.x, en.pos.y + en.h, colour_scatter_back,
											en.pos.x - en.w, en.pos.y + en.h / 2, colour_scatter_front, 0);
						break;
					case ENEMY_TYPE_AIMER:
						C2D_DrawCircle(en.pos.x, en.pos.y, 0, en.w,
										colour_aimer_tl, colour_aimer_br, colour_aimer_bl, colour_aimer_br);
						break;
					case ENEMY_TYPE_MACHINE:
						if (cur.machine.shooting) {
							C2D_DrawRectSolid(en.pos.x - en.w / 4, en.pos.y + en.h / 4, 0, en.w / 2, en.h / 2, colour_machine_active);
						} else {
							C2D_DrawRectSolid(en.pos.x - en.w / 4, en.pos.y + en.h / 4, 0, en.w / 2, en.h / 2, colour_blend(colour_machine_not_active, colour_machine_active, cur.machine.cooldown / 90.0f));
						}
						C2D_DrawRectSolid(en.pos.x, en.pos.y, 0, en.w, en.h, colour_machine_base);
						break;
					case ENEMY_TYPE_HAILER:
						C2D_DrawTriangle(en.pos.x, en.pos.y, colour_hailer_shooter,
										en.pos.x - en.w, en.pos.y - en.h / 1.5, colour_hailer_shooter_top,
										en.pos.x, en.pos.y - en.h * 1.2 , colour_hailer_shooter_top, 0);
						C2D_DrawCircleSolid(en.pos.x, en.pos.y, 0, en.w, colour_hailer_base);
						break;
					case ENEMY_TYPE_BOSS:
						boss_render(&cur);
						break;
					default:
						break;
				}
			}

			// Draw bullets
			for (int i = 0; i < num_bullets; i++) {
				bullet cur = bullets[i];
				switch (cur.from) {
					case ENEMY_TYPE_NONE:
					case ENEMY_TYPE_SCATTER:
					case ENEMY_TYPE_MACHINE:
						C2D_DrawRectSolid(cur.pos.x, cur.pos.y, 0, 2, 2, colour_white);
						break;
					case ENEMY_TYPE_AIMER:
						C2D_DrawCircleSolid(cur.pos.x, cur.pos.y, 0, 4, colour_aimer_bullet);
						break;
					case ENEMY_TYPE_HAILER:
						C2D_DrawEllipseSolid(cur.pos.x, cur.pos.y, 0, 4, 2, colour_hailer_bullet);
						break;
					case ENEMY_TYPE_BOSS:
						C2D_DrawCircleSolid(cur.pos.x, cur.pos.y, 0, 4, colour_red);
						break;
					default:
						break;
				}
			}

		} else if (state == STATE_LEVEL_WIN) {
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, colour_clear);
			C2D_SceneBegin(top);

			if (since_state_change < 2) {
				for (int i = 0; i < num_bonuses; i++) {
					sprintf(valid_bonuses[i].name, "Clear");
					valid_bonuses[i].active = false;
					valid_bonuses[i].value = 0;
				}

				num_valid_bonuses = 0;
				for (int i = 0; i < num_bonuses; i++) {
					if (bonuses[i].active) {
						valid_bonuses[num_valid_bonuses].active = true;
						valid_bonuses[num_valid_bonuses].value = bonuses[i].value;
						sprintf(valid_bonuses[num_valid_bonuses].name, "%s", bonuses[i].name);
						num_valid_bonuses++;
					}
				}

				int total_health = 0;
				for (int i = 0; i < levels[current_level - 1].num_waves; i++) {
					wave cur = levels[current_level - 1].waves[i];
					if (cur.flags & WAVE_TYPE_ENEMY) {
						for (int j = 0; j < cur.enemy_wave.num_enemies; j++) {
							switch (cur.enemy_wave.enemies[j]) {
								case ENEMY_TYPE_SCATTER:
									total_health += SCATTER_HEALTH;
									break;
								case ENEMY_TYPE_AIMER:
									total_health += AIMER_HEALTH;
									break;
								case ENEMY_TYPE_MACHINE:
									total_health += MACHINE_HEALTH;
									break;
								case ENEMY_TYPE_HAILER:
									total_health += HAILER_HEALTH;
									break;
								default:
									break;
							}
						}
					}
					if (cur.flags & WAVE_TYPE_BOSS) {
						for (int j = 0; j < cur.boss_wave.num_bosses; j++) {
							switch (cur.boss_wave.bosses[j]) {
								case BOSS_TYPE_PROG:
									total_health += PROG_HEALTH;
									break;
								default:
									break;
							}
						}
					}
				}
				int bullets = total_health / 10;
				if (bullets_fired >= bullets * 1.25f) {
					set_bonus_active("Cheapskate", false);
				}
				if (bullets_fired >= bullets * 6) {
					set_bonus_active("Triggerhappy", true);
				}
			}

			if (since_state_change <= seconds(1.5f)) {
				C2D_DrawText(&win_text, 0, TOP_SCREEN_WIDTH / 2 - win_text.width / 2, TOP_SCREEN_HEIGHT / 2, 0, 1, 1);
			} else if (since_state_change <= seconds(3.0f)) {
				float to_move = TOP_SCREEN_HEIGHT / 2 - 16;
				float percentage = 1 - (float)(since_state_change - seconds(1.5f)) / seconds(1.5f);
				C2D_DrawText(&win_text, 0, TOP_SCREEN_WIDTH / 2 - win_text.width / 2, 8 + to_move * percentage, 0, 1, 1);
			} else if (since_state_change <= seconds(3.5f)) {
				C2D_DrawText(&win_text, 0, TOP_SCREEN_WIDTH / 2 - win_text.width / 2, 8, 0, 1, 1);
			} else if (since_state_change <= seconds(4.5f)) {
				C2D_DrawText(&win_text, 0, TOP_SCREEN_WIDTH / 2 - win_text.width / 2, 8, 0, 1, 1);
				char *text = malloc(sizeof(*text) * 64);
				sprintf(text, "Score: %d", score);
				draw_text(text, TOP_SCREEN_WIDTH / 2, 40, 1, 1, NULL, NULL);
				free(text);
			} else {
				C2D_DrawText(&win_text, 0, TOP_SCREEN_WIDTH / 2 - win_text.width / 2, 8, 0, 1, 1);
				char *text = malloc(sizeof(*text) * 64);
				sprintf(text, "Score: %d", score);
				draw_text(text, TOP_SCREEN_WIDTH / 2, 40, 1, 1, NULL, NULL);
				free(text);
				int time_since_4s = since_state_change - seconds(4.5f);
				float secs_over_4 = time_since_4s / FRAMERATE;
				int number_of_bonuses = secs_over_4;
				int offset = 0;
				final_score = score;
				if (number_of_bonuses <= num_valid_bonuses) {
					if (number_of_bonuses < 3) {
						for (int i = 0; i < number_of_bonuses; i++) {
							bonus current = valid_bonuses[i];
							text = malloc(sizeof(*text) * 64);
							if (!current.active) {
								sprintf(text, "%s - 0", current.name);
							} else {
								sprintf(text, "%s - %d", current.name, current.value);
							}
							float h;
							draw_text(text, TOP_SCREEN_WIDTH / 2, 80 + offset + (i * 8), 1, 1, NULL, &h);
							offset += h;
							free(text);
						}
					} else {
						int real_count = 0;
						for (int i = number_of_bonuses - 3; i < number_of_bonuses; i++) {
							bonus current = valid_bonuses[i];
							text = malloc(sizeof(*text) * 64);
							if (!current.active) {
								sprintf(text, "%s - 0", current.name);
							} else {
								sprintf(text, "%s - %d", current.name, current.value);
							}
							float h;
							draw_text(text, TOP_SCREEN_WIDTH / 2, 80 + offset + (real_count * 8), 1, 1, NULL, &h);
							offset += h;
							free(text);
							real_count++;
						}
					}
				} else {
					if (num_valid_bonuses < 3) {
						for (int i = 0; i < num_valid_bonuses; i++) {
							bonus current = valid_bonuses[i];
							text = malloc(sizeof(*text) * 64);
							if (!current.active) {
								sprintf(text, "%s - 0", current.name);
							} else {
								sprintf(text, "%s - %d", current.name, current.value);
							}
							float h;
							draw_text(text, TOP_SCREEN_WIDTH / 2, 80 + offset + (i * 8), 1, 1, NULL, &h);
							offset += h;
							free(text);
						}
					} else {
						int real_count = 0;
						for (int i = num_valid_bonuses - 3; i < num_valid_bonuses; i++) {
							bonus current = valid_bonuses[i];
							text = malloc(sizeof(*text) * 64);
							if (!current.active) {
								sprintf(text, "%s - 0", current.name);
							} else {
								sprintf(text, "%s - %d", current.name, current.value);
							}
							float h;
							draw_text(text, TOP_SCREEN_WIDTH / 2, 80 + offset + (real_count * 8), 1, 1, NULL, &h);
							offset += h;
							free(text);
							real_count++;
						}
					}

					for (int i = 0; i < num_valid_bonuses; i++) {
						if (bonuses[i].active) final_score += bonuses[i].value;
					}

					int time_over_final = secs_over_4 - num_valid_bonuses;
					if (time_over_final > 1) {
						if (time_over_final <= 5) {
							text = malloc(sizeof(*text) * 64);
							sprintf(text, "Final Score: %d", final_score);
							draw_text(text, TOP_SCREEN_WIDTH / 2, TOP_SCREEN_HEIGHT - 32, 1, 1, NULL, NULL);
							free(text);
						} else {
							setState(STATE_MAP);
							pl.health = pl.max_health;
							while (num_bullets > 0) { // Clear all bullets
								delete_bullet(0);
							}
							while (num_enemies > 0) {
								remove_enemy(0);
							}
						}
					}
				}
			}
		}

		C2D_TargetClear(bottom, colour_clear);
		C2D_SceneBegin(bottom);
		C3D_FrameDrawOn(bottom);

		// Draw level meter
		if (state == STATE_LEVEL) {
			char *text = malloc(sizeof(*text) * 64);
			float h;
			float height_off = 0;
			sprintf(text, "Score: %d", score);
			draw_text(text, BOTTOM_SCREEN_WIDTH / 2, 32 + height_off, 1, 1, NULL, &h);
			height_off += h;

			float bar_width = 2 * BOTTOM_SCREEN_WIDTH / 3;
			C2D_DrawRectSolid(BOTTOM_SCREEN_WIDTH / 6 - 4, 8, 0, bar_width + 8, 16, colour_white);
			level lv = levels[current_level - 1];
			float total_duration = 0;
			for (int i = 0; i < lv.num_waves; i++) {
				total_duration += lv.waves[i].duration;
			}
			total_duration = seconds(total_duration);
			float width_per_duration = bar_width / total_duration;

			if (speed) {
				since_state_change += 2;
				score += 2;
			}
			int frames_since_spawn = frame_num - last_wave_spawn;
			float width = 0;
			for (int i = 0; i < next_wave - 1; i++) {
				width += seconds(lv.waves[i].duration) * width_per_duration;
			}
			float width_for_wave = seconds(lv.waves[next_wave - 1].duration) * width_per_duration;
			float percentage = ((float)frames_since_spawn / seconds(lv.waves[next_wave - 1].duration));
			C2D_DrawRectSolid(BOTTOM_SCREEN_WIDTH / 6, 10, 0, width + width_for_wave * (percentage <= 1 ? percentage : 1), 12, colour_pl_front);

			float offset = 0;
			for (int i = 0; i < lv.num_waves; i++) {
				width = seconds(lv.waves[i].duration) * width_per_duration;
				C2D_DrawRectSolid(BOTTOM_SCREEN_WIDTH / 6 + offset - 2, 6, 0, 4, 20, colour_red);
				offset += width;
			}

			if (next_wave >= lv.num_waves) {
				if ((float)frames_since_spawn / seconds(lv.waves[next_wave - 1].duration) > 1) {
					sprintf(text, "Out of Time! Poison Activated!");
					float width, height;
					C2D_Text text_obj;
					C2D_TextBuf buf = C2D_TextBufNew(strlen(text));
					C2D_TextFontParse(&text_obj, font, buf, text);
					C2D_TextOptimize(&text_obj);
					C2D_TextGetDimensions(&text_obj, 0.75, 0.75, &width, &height);
					C2D_DrawText(&text_obj, C2D_WithColor | C2D_AtBaseline, BOTTOM_SCREEN_WIDTH / 2 - width / 2, BOTTOM_SCREEN_HEIGHT - 16, 0, 0.75, 0.75, frame_num % FRAMERATE * 1.5 < 10 ? C2D_Color32(0xef, 0x23, 0x3c, 0xff) : C2D_Color32(0xfa, 0xa3, 0x07, 0xff));
					C2D_TextBufDelete(buf);

					float perc_over = ((float)frames_since_spawn / seconds(lv.waves[next_wave - 1].duration)) - 1;
					pl.health -= pl.max_health * (perc_over / 250.0f);
					if (pl.health <= 0) {
						setState(STATE_LOSE_TIME);
						pl.health = pl.max_health;
					}
				} else if ((float)frames_since_spawn / seconds(lv.waves[next_wave - 1].duration) >= 0.75) {
					sprintf(text, "Low Time!");
					float width, height;
					C2D_Text text_obj;
					C2D_TextBuf buf = C2D_TextBufNew(strlen(text));
					C2D_TextFontParse(&text_obj, font, buf, text);
					C2D_TextOptimize(&text_obj);
					C2D_TextGetDimensions(&text_obj, 1, 1, &width, &height);
					C2D_DrawText(&text_obj, C2D_WithColor | C2D_AtBaseline, BOTTOM_SCREEN_WIDTH / 2 - width / 2, BOTTOM_SCREEN_HEIGHT - 16, 0, 1, 1, C2D_Color32(0xef, 0x23, 0x3c, 0xff));
					C2D_TextBufDelete(buf);
				}
			}
			free(text);
		}

		C3D_FrameEnd(0);
		since_state_change++;
		delay();
	}

	C2D_FontFree(font);
	C2D_SpriteSheetFree(sprite_sheet);
	C2D_TextBufDelete(g_staticBuf);

	C2D_Fini();
	C3D_Fini();
	hidExit();
	aptExit();
	srvExit();
	gfxExit();
	romfsExit();
	return 0;
}

void draw_text(char *text, int x, int y, int scalex, int scaley, float *width, float *height) {
	C2D_Text text_obj;
	C2D_TextBuf buf = C2D_TextBufNew(strlen(text));
	C2D_TextFontParse(&text_obj, font, buf, text);
	C2D_TextOptimize(&text_obj);
	float w, h;
	if (!width) width = &w;
	if (!height) height = &h;
	C2D_TextGetDimensions(&text_obj, scalex, scaley, width, height);
	C2D_DrawText(&text_obj, 0, x - (w / 2.0f), y, 0, scalex, scaley);
	C2D_TextBufDelete(buf);
}

void set_bonus_active(char *name, bool active) {
	for (int i = 0; i < num_bonuses; i++) {
		if (strcmp(name, bonuses[i].name) == 0) {
			bonuses[i].active = active;
			return;
		}
	}
}