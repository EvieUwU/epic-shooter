typedef struct level {
	int num_waves;
	wave waves[64];
} level;

extern bool speed;

void level_spawner(int current_lv, level levels[]);
void level_bg(int level);