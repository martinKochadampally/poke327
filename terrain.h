#ifndef TERRAIN_H
#define TERRAIN_H

#include "queue.h"

#define HEIGHT 21
#define WIDTH 80

int clear_map(char map[HEIGHT][WIDTH]);
int place_roads(char map[HEIGHT][WIDTH], int direction, int p1, int p2, int intersection);
int seed_map(char map[HEIGHT][WIDTH], queue *q);
int init_borders(char map[HEIGHT][WIDTH]);
int place_building(char map[HEIGHT][WIDTH],int road, int p1, int p2, int direction, char symbol);
int print_map(char map[HEIGHT][WIDTH]);

#endif