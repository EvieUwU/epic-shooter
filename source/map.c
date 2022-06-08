#include "main.h"

int selected_map = 0;
bool selected_map_changed = false;
int since_map_change = 0;
int last_selected_map = 0;
bool won_levels[5] = {false, false, false, false, false};

void draw_map(bool unlocked) {
	bool up = true;
	int radius = 20;
	float top = ceil(num_levels / 2);
	int start_offset = 25;
	int gap_between = (TOP_SCREEN_WIDTH - start_offset * 2) / top / 2;
	for (int i = 0; i < num_levels; i++) {
		int vertical = TOP_SCREEN_HEIGHT / 4;
		int inverse_vertical = vertical * 3;
		if (!up) {
			vertical = 3 * TOP_SCREEN_HEIGHT / 4;
			inverse_vertical = TOP_SCREEN_HEIGHT / 4;
		}
		if (i + 1 < num_levels) {
			C2D_DrawLine(start_offset + i * gap_between, vertical, colour_black, start_offset + (i + 1) * gap_between, inverse_vertical, colour_black, 5, 0);
		}
		if (won_levels[i])
			C2D_DrawCircle(start_offset + i * gap_between, vertical, 0, radius, colour_dark_green, colour_green, colour_green, colour_dark_green);
		else
			C2D_DrawCircle(start_offset + i * gap_between, vertical, 0, radius, colour_dark_red, colour_red, colour_red, colour_dark_red);
		up = !up;
		if (i == selected_map) {
			if (!selected_map_changed)
				C2D_DrawText(&level_text[i], 0, TOP_SCREEN_WIDTH / 2 - level_text[i].width / 2, 5, 0, 1, 1);
			int h = pl.h;
			int w = pl.w;
			float x = start_offset + i * gap_between - w / 2;
			float y = vertical - h / 2;
			if (selected_map_changed) {
				if (since_map_change > seconds(1)) {
					selected_map_changed = false;
				} else {
					float x_start = x;
					float y_start = y;
					float x_end = start_offset + last_selected_map * gap_between - w / 2;
					float y_end = inverse_vertical - h / 2;
					float x_diff = x_end - x_start;
					float y_diff = y_end - y_start;
					x = x_end - x_diff * ((float)since_map_change / (float)seconds(1));
					y = y_end - y_diff * ((float)since_map_change / (float)seconds(1));
				}
			}
			C2D_DrawTriangle(	x + w, y + h / 2, colour_pl_front, 
								x, y, colour_pl_back,
								x + w / 4, y + h / 2, colour_pl_mid, 1);
			C2D_DrawTriangle(	x + w / 4, y + h / 2, colour_pl_mid,
								x, y + h, colour_pl_back,
								x + w, y + h / 2, colour_pl_front, 1);
		}
	}
	if (unlocked) {
		C2D_DrawText(&unlocked_text, C2D_AtBaseline, 0, TOP_SCREEN_HEIGHT, 0, 1, 1);
	}
	printf("\x1b[2;1HFrame num: %d\x1b[K", frame_num);
}