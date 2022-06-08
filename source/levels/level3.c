#include "../main.h"

int level3_num_waves = 8;
wave level3_waves[8] = {
	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 8,
		.enemy_wave = {
			.enemies = {ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER},
			.num_enemies = 3,
		}
	},

	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 12,
		.enemy_wave = {
			.enemies = {ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER},
			.num_enemies = 5,
		}
	},

	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 12,
		.enemy_wave = {
			.enemies = {ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER,
						ENEMY_TYPE_SCATTER},
			.num_enemies = 6,
		}
	},

	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 17,
		.enemy_wave = {
			.enemies = {ENEMY_TYPE_MACHINE, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER,
						ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER},
			.num_enemies = 8,
		}
	},

	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 18,
		.enemy_wave = {
			.enemies = {ENEMY_TYPE_MACHINE, ENEMY_TYPE_HAILER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER,
						ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER},
			.num_enemies = 8,
		}
	},

	{
		.flags = WAVE_TYPE_ENEMY,
		.duration = 30,
		.enemy_wave = {
			.enemies = {ENEMY_TYPE_MACHINE, ENEMY_TYPE_MACHINE, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER,
						ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER, ENEMY_TYPE_SCATTER,
						ENEMY_TYPE_AIMER, ENEMY_TYPE_AIMER},
			.num_enemies = 12,
		}
	},
};