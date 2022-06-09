#include "main.h"
/**
 * @brief Draws a quadrilateral.
 * 
 * @param a Point A
 * @param a_colour Colour for point A
 * @param b Point B
 * @param b_colour Colour for point B
 * @param c Point C
 * @param c_colour Colour for point C
 * @param d Point D
 * @param d_colour Colour for point D
 */
void draw_quad(point a, u32 a_colour, point b, u32 b_colour, point c, u32 c_colour, point d, u32 d_colour) {
	C2D_DrawTriangle(a.x, a.y, a_colour, b.x, b.y, b_colour, c.x, c.y, c_colour, 0);
	C2D_DrawTriangle(b.x, b.y, b_colour, c.x, c.y, c_colour, d.x, d.y, d_colour, 0);
}

/**
 * @brief Draws a coloured background
 * 
 * @param tl Colour for the top left
 * @param tr Colour for the top right
 * @param bl Colour for the bottom left
 * @param br Colour for the bottom right
 */
void background(u32 tl, u32 tr, u32 bl, u32 br) {
	draw_quad((point){0, 0}, tl, (point){TOP_SCREEN_WIDTH, 0}, tr, (point){0, TOP_SCREEN_HEIGHT}, bl, (point){TOP_SCREEN_WIDTH, TOP_SCREEN_HEIGHT}, br);
}

/**
 * @brief Draws a background with a solid colour
 * 
 * @param colour Colour to make the background
 */
void background_solid(u32 colour) {
	background(colour, colour, colour, colour);
}

void draw_enemy(enemy cur) {
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

void draw_bullet(bullet cur) {
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
			C2D_DrawCircleSolid(cur.pos.x, cur.pos.y, 0, 3, colour_hailer_bullet);
			break;
		case ENEMY_TYPE_BOSS:
			C2D_DrawCircleSolid(cur.pos.x, cur.pos.y, 0, 4, colour_red);
			break;
		default:
			break;
	}
}