#include "../main.h"

int level1_num_waves = 1;
wave level1_waves[1] = {
	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 10,
		.enemy_wave = {
			.num_enemies = 3,
			.enemies = {ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER},
		}
	},

	/*{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 12,
		.enemy_wave = {
			.num_enemies = 5,
			.enemies = {ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER},
		}
	},

	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 13,
		.enemy_wave = {
			.num_enemies = 6,
			.enemies = {ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,
						ENEMY_TYPE_SCATTER},
		}
	},

	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 16,
		.enemy_wave = {
			.num_enemies = 8,
			.enemies = {ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,
						ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER},
		}
	},

	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 14,
		.enemy_wave = {
			.num_enemies = 5,
			.enemies = {ENEMY_TYPE_AIMER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER,	ENEMY_TYPE_SCATTER},
		}
	},

	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 15,
		.enemy_wave = {
			.num_enemies = 3,
			.enemies = {ENEMY_TYPE_AIMER,	ENEMY_TYPE_AIMER,	ENEMY_TYPE_SCATTER},
		},
	},*/
};