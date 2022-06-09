typedef struct level {
	u32 high_score;
	int num_waves;
	wave waves[64];
} level;

extern bool speed;

extern int SCATTER_HEALTH;
extern int AIMER_HEALTH;
extern int MACHINE_HEALTH;
extern int HAILER_HEALTH;

extern int PROG_HEALTH;

void level_spawner(int current_lv, level levels[]);
void level_bg(int level);