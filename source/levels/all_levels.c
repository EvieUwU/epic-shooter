#include "../main.h"

void levelsInit(level levels[]) {
	levels[0].num_waves = level1_num_waves;
	levels[2].high_score = 0;
	for (int i = 0; i < level1_num_waves; i++) {
		levels[0].waves[i] = level1_waves[i];
	}

	levels[1].num_waves = level2_num_waves;
	levels[2].high_score = 0;
	for (int i = 0; i < level2_num_waves; i++) {
		levels[1].waves[i] = level2_waves[i];
	}

	levels[2].num_waves = level3_num_waves;
	levels[2].high_score = 0;
	for (int i = 0; i < level3_num_waves; i++) {
		levels[2].waves[i] = level3_waves[i];
	}
}