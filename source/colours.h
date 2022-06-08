#define colour_rgba(r, g, b, a) \
	(r | (g << (u32)8) | (b << (u32)16) | (a << (u32)24));

#define colour_rgb(r, g, b) \
	colour_rgba(r, g, b, 0xFF);

#define from_hex(hex) \
	colour_rgb(hex >> 16, (hex >> 8) & 0xFF, hex & 0xFF);

extern u32 colour_pl_front;
extern u32 colour_pl_back;
extern u32 colour_pl_mid;

extern u32 colour_scatter_front;
extern u32 colour_scatter_back;
extern u32 colour_scatter_mid;

extern u32 colour_aimer_tl;
extern u32 colour_aimer_tr;
extern u32 colour_aimer_bl;
extern u32 colour_aimer_br;
extern u32 colour_aimer_bullet;

extern u32 colour_machine_base;
extern u32 colour_machine_not_active;
extern u32 colour_machine_active;

extern u32 colour_hailer_base;
extern u32 colour_hailer_shooter;
extern u32 colour_hailer_shooter_top;
extern u32 colour_hailer_bullet;

extern u32 colour_black;
extern u32 colour_white;
extern u32 colour_red;
extern u32 colour_green;
extern u32 colour_blue;
extern u32 colour_yellow;
extern u32 colour_cyan;
extern u32 colour_magenta;
extern u32 colour_grey;
extern u32 colour_dark_grey;
extern u32 colour_dark_red;
extern u32 colour_dark_green;
extern u32 colour_dark_blue;
extern u32 colour_dark_yellow;
extern u32 colour_dark_cyan;
extern u32 colour_dark_magenta;
extern u32 colour_dark_orange;

extern u32 colour_clear;

u32 colour_blend(u32 start_colour, u32 end_colour, float percent);