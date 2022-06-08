typedef enum enemy_type {
	ENEMY_TYPE_NONE = 0,
	ENEMY_TYPE_BOSS = 1,
	ENEMY_TYPE_SCATTER = 2,
	ENEMY_TYPE_AIMER = 3,
	ENEMY_TYPE_MACHINE = 4,
	ENEMY_TYPE_HAILER = 5,
} enemy_type;

typedef enum boss_type {
	BOSS_TYPE_PROG,
} boss_type;

typedef struct enemy_scatter {
	point oldpos;
	point newpos;
} enemy_scatter;

typedef struct enemy_aimer {
	bool going_up;
	float percentage;
} enemy_aimer;

typedef struct enemy_machine {
	bool shooting;
	int cooldown;
} enemy_machine;

typedef struct enemy_hailer {
	float cooldown;
} enemy_hailer;

typedef struct boss_prog {
	bool going_up;
	float percentage;
	int phase;
	int shots;
	int shooting_cooldown;
	bool wait;
} boss_prog;

typedef struct enemy_boss {
	boss_type type;
	int cooldown;
	float rotation;
	C2D_Image sprite;
	union {
		boss_prog prog;
	};
} enemy_boss;

typedef struct enemy_data {
	point pos;
	int w, h;
	int health;
	int max_health;
	int since_last_action;
	int actions;
} enemy_data;

typedef struct enemy {
	enemy_type type;
	enemy_data data;
	union {
		enemy_scatter scatter;
		enemy_aimer aimer;
		enemy_machine machine;
		enemy_hailer hailer;
		enemy_boss boss;
	};
} enemy;

typedef struct wave_enemy {
	int num_enemies;
	enemy_type enemies[64];
} wave_enemy;

typedef struct wave_boss {
	int num_bosses;
	boss_type bosses[64];
} wave_boss;

typedef enum wave_type {
	WAVE_TYPE_ENEMY = 1,
	WAVE_TYPE_BOSS = 2,
} wave_type;

typedef struct wave {
	int flags;
	wave_enemy enemy_wave;
	wave_boss boss_wave;
	float duration;
} wave;

extern enemy enemies[MAX_ENEMIES];
extern int num_enemies;

void create_enemy(enemy_type type, point pos, int w, int h, int health);
void remove_enemy(int idx);