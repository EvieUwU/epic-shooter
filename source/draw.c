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