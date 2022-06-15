#pragma once

#define TOP_SCREEN_WIDTH  400
#define TOP_SCREEN_HEIGHT 240

#define BOTTOM_SCREEN_WIDTH 320
#define BOTTOM_SCREEN_HEIGHT 240

#define CSTICK_DEAD_ZONE 12

#define FRAMERATE 60
#define MS_PER_FRAME 1000 / FRAMERATE

#define BULLET_SPEED 5
#define MAX_ENEMIES 512
#define MAX_BULLETS 1024
#define MAX_SPRITES 768

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <citro2d.h>
#include <time.h>

typedef enum game_state {
	STATE_MAP = 0,
	STATE_LEVEL = 1,
	STATE_GAME_OVER = 2,
	STATE_LEVEL_WIN = 3,
	STATE_BOSS_LEVEL = 4,
	STATE_DEAD = 5,
	STATE_LOSE_TIME = 6,
} game_state;

typedef struct point {
	float x, y;
} point;

typedef struct line {
	point start, end;
	float m, c;
} line;

typedef struct player {
	point pos;
	int w, h;
	int max_health;
	float speed;
	float health;
	float last_regen;
} player;

typedef struct bonus {
	char name[32];
	int value;
	bool active;
} bonus;

typedef struct option {
	C2D_Text text;
	void (*cb) ();
	u32 selectedColour;
} option;

#include "colours.h"
#include "enemy.h"

typedef struct bullet {
	point pos;
	point vel;
	enemy_type from;
} bullet;

#include "draw.h"
#include "behaviour.h"
#include "map.h"
#include "levels.h"

#include "save.h"

#include "levels/boss_logic.h"

#include "levels/level1.h"
#include "levels/level2.h"
#include "levels/level3.h"
#include "levels/all_levels.h"

bool inside_tri(point p, point tri_1, point tri_2, point tri_3);
bool inside_rect(point p, point top_left, int w, int h);

circlePosition readCStick(int dead_zone);
float sign (point p1, point p2, point p3);
void create_bullet(float x, float y, enemy_type from, float vel_x, float vel_y);
void delete_bullet(int idx);
int seconds(float secs);
void setState(game_state new_state);
float dist(point a, point b);
float dist_sq(point a, point b);

extern circlePosition pos;

extern C3D_RenderTarget *top;
extern C3D_RenderTarget *bottom;

extern int num_bullets;
extern bullet bullets[MAX_BULLETS];

extern int last_bullet;
extern int frame_num;

extern player pl;

extern enemy enemies[MAX_ENEMIES];
extern int num_enemies;

extern int next_level;
extern int time_since_last_enemy_spawn;

extern bool paused;

extern u64 now_time;
extern u64 last_time;

extern C2D_SpriteSheet sprite_sheet;
extern C2D_Sprite sprites[MAX_SPRITES / 2];
extern C2D_Sprite boss_sprites[MAX_SPRITES / 2];
extern size_t num_sprites;

extern game_state state;
extern int since_state_change;

extern C2D_Text level_text[5];
extern int num_levels;
extern level levels[5];

extern int last_wave_spawn;
extern int next_wave;

extern int score;
extern int bullet_damage;

extern C2D_Text unlocked_text;

void draw_text(char *text, int x, int y, int scalex, int scaley, float *width, float *height);
void set_bonus_active(char *name, bool active);