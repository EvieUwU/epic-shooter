#include "main.h"

u32 colour_pl_front = colour_rgb(0xFF, 0xBA, 0x08);
u32 colour_pl_back = colour_rgb(0xDC, 0x2F, 0x02);
u32 colour_pl_mid = colour_rgb(0xF4, 0x8C, 0x06);

u32 colour_scatter_front = colour_rgb(0xA4, 0x13, 0x3C);
u32 colour_scatter_back = colour_rgb(0xFF, 0x4D, 0x6D);
u32 colour_scatter_mid = colour_rgb(0xC9, 0x18, 0x4A);

u32 colour_aimer_tl = colour_rgb(0x00, 0x7F, 0x5F);
u32 colour_aimer_tr = colour_rgb(0x2B, 0x93, 0x48);
u32 colour_aimer_bl = colour_rgb(0x2B, 0x93, 0x48);
u32 colour_aimer_br = colour_rgb(0x55, 0xA6, 0x30);
u32 colour_aimer_bullet = colour_rgb(0xBF, 0xD2, 0x00);

u32 colour_machine_base = colour_rgb(0xE9, 0xD8, 0xA6);
u32 colour_machine_not_active = colour_rgb(0xB5, 0xE4, 0x8C);
u32 colour_machine_active = colour_rgb(0xAE, 0x20, 0x12);

u32 colour_hailer_base = colour_rgb(0xBD, 0xE0, 0xFE);
u32 colour_hailer_shooter = colour_rgb(0xA2, 0xD2, 0xFF);
u32 colour_hailer_shooter_top = colour_rgb(0x00, 0x6D, 0x77);
u32 colour_hailer_bullet = colour_rgb(0xF9, 0x84, 0x4A);

u32 colour_black = colour_rgb(0x00, 0x00, 0x00);
u32 colour_white = colour_rgb(0xFF, 0xFF, 0xFF);
u32 colour_red = colour_rgb(0xEF, 0x23, 0x3C);
u32 colour_green = colour_rgb(0x00, 0xFF, 0x00);
u32 colour_blue = colour_rgb(0x00, 0x00, 0xFF);
u32 colour_yellow = colour_rgb(0xFF, 0xFF, 0x00);
u32 colour_cyan = colour_rgb(0x00, 0xFF, 0xFF);
u32 colour_magenta = colour_rgb(0xFF, 0x00, 0xFF);
u32 colour_grey = colour_rgb(0x80, 0x80, 0x80);
u32 colour_dark_grey = colour_rgb(0x40, 0x40, 0x40);
u32 colour_dark_red = colour_rgb(0x80, 0x00, 0x00);
u32 colour_dark_green = colour_rgb(0x00, 0x80, 0x00);
u32 colour_dark_blue = colour_rgb(0x00, 0x00, 0x80);
u32 colour_dark_yellow = colour_rgb(0x80, 0x80, 0x00);
u32 colour_dark_cyan = colour_rgb(0x00, 0x80, 0x80);
u32 colour_dark_magenta = colour_rgb(0x80, 0x00, 0x80);
u32 colour_dark_orange = colour_rgb(0x80, 0x40, 0x00);

u32 colour_clear = colour_rgb(0x80, 0x6C, 0x58);

u32 colour_blend(u32 start_colour, u32 end_colour, float percent) {
	u8 a_start = (start_colour) & 0xFF;
	u8 b_start = (start_colour >> 8) & 0xFF;
	u8 g_start = (start_colour >> 16) & 0xFF;
	u8 r_start = (start_colour >> 24) & 0xFF;
	u8 r_diff = (end_colour & 0xFF) - r_start;
	u8 g_diff = ((end_colour >> 8) & 0xFF) - g_start;
	u8 b_diff = ((end_colour >> 16) & 0xFF) - b_start;
	u8 a_diff = ((end_colour >> 24) & 0xFF) - a_start;
	u8 r = r_start + r_diff * percent;
	u8 g = g_start + g_diff * percent;
	u8 b = b_start + b_diff * percent;
	u8 a = a_start + a_diff * percent;
	return colour_rgba(r, g, b, a);
}