void boss_logic(enemy *en);
bool boss_collision(bullet *cur, enemy *en);
void boss_spawn(boss_type type, int x, int y, int w, int h, int health);
void boss_render(enemy *en);
void boss_prog_logic(enemy *en);