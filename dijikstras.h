#ifndef DIJIKSTRAS_H
#define DIJIKSTRAS_H

#include "map.h"

extern int cost_table[5][11];

int dijiksra(int dist_map[HEIGHT][WIDTH], map *m, terrain *pc_pos, enum char_type npc);

#endif