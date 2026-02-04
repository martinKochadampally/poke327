#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "queue.h"

#define HEIGHT 21
#define WIDTH 80

/* Sets everything on the map to \0 to make seeding easier. */
int clear_map(char map[HEIGHT][WIDTH]) {
    int i, j;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            map[i][j] = '\0';
        }
    }

    return 0;
}

/* This code given the 2 endpoints of the path and where they are supposed to meet, 
generates a road with a bend at intersection */
int place_roads(char map[HEIGHT][WIDTH], int direction, int p1, int p2, int intersection) {
    int i;
    int HorW = direction ? HEIGHT : WIDTH;  

    for (i = 0; i < intersection; i++) {
        if(direction) {
            map[i][p1] = '#';
            continue;
        }
        map[p1][i] = '#';
    }

    i = p1;

    while (i != p2) {
        if (direction) {
            map[intersection][i] = '#';
        } else {
            map[i][intersection] = '#';
        }
        
        if (p2 < p1) {
            i--;
            continue;
        }
        i++;
    }

    for (i = intersection; i < HorW; i++) {
        if(direction) {
            map[i][p2] = '#';
            continue;
        }
        map[p2][i] = '#';
    }

    return 0;
}

/* This function takes the map an the queue that has
already been seeded in poke327.c and expands the seed to 
all neigbouring unwritten space.
By unwritten I mean with value of '\0'*/
int seed_map(char map[HEIGHT][WIDTH], queue *q) {
    int x, y, size;

    while (!queue_size(q, &size) && size) {
        queue_dequeue(q, &x, &y);

        if (y-1 > -1 && x-1 > -1 && !map[y-1][x-1]){
            map[y-1][x-1] = map[y][x];
            if (queue_enqueue(q, x-1, y-1)) {
                return -1;
            }
        }
        if (y-1 > -1 && !map[y-1][x]){
            map[y-1][x] = map[y][x];
            if (queue_enqueue(q, x, y-1)) {
                return -1;
            }
        }
        if (y-1 > -1 && x+1 < WIDTH && !map[y-1][x+1]){
            map[y-1][x+1] = map[y][x];
            if (queue_enqueue(q, x+1, y-1)) {
                return -1;
            }
        }
        if (x+1 < WIDTH && !map[y][x+1]){
            map[y][x+1] = map[y][x];
            if (queue_enqueue(q, x+1, y)) {
                return -1;
            }
        }
        if (y+1 < HEIGHT && x+1 < WIDTH && !map[y+1][x+1]){
            map[y+1][x+1] = map[y][x];
            if (queue_enqueue(q, x+1, y+1)) {
                return -1;
            }
        }
        if (y+1 < HEIGHT && x && !map[y+1][x]){
            map[y+1][x] = map[y][x];
            if (queue_enqueue(q, x, y+1)) {
                return -1;
            }

        }
        if (y+1 < HEIGHT && x-1 > -1 && !map[y+1][x-1]){
            map[y+1][x-1] = map[y][x];
            if (queue_enqueue(q, x-1, y+1)) {
                return -1;
            }
        }
        if (y && x-1 > -1 && !map[y][x-1]){
            map[y][x-1] = map[y][x];
            if (queue_enqueue(q, x-1, y)) {
                return -1;
            }
        }
    }

    return 0;
}

int init_borders(char map[HEIGHT][WIDTH]){
    int i, j, m;

    i = 0;
    j = HEIGHT - 1; 
    for (m = 0; m < WIDTH; m++) {
        map[i][m] = (map[i][m] == '#'? '#':'%');
        map[j][m] = (map[j][m] == '#'? '#':'%');
    }

    i = 0;
    j = WIDTH - 1; 
    for (m = 0; m < HEIGHT; m++) {
        map[m][i] = (map[m][i] == '#'? '#':'%');
        map[m][j] = (map[m][j] == '#'? '#':'%');
    }
    return 0;
}

/* This code places the 2x2 square for the pokemon center. 
It does this by randomly picking a point on the first half of the road
and placing the spcified building above it or below it if there isnt space. 

direction indicates whether this is the vertical road or horizontal road. */
int place_building(char map[HEIGHT][WIDTH],int road, int p1, int p2, int direction, char symbol){
    int tmp;
    int x, y;

    do {
        x = p1 + 1 + rand()%(p2-p1-2);
        if (road > HEIGHT/4) {
            y = road - 2;
        }   else {
            y = road + 1;
        }
        if (direction) {
            tmp = x;
            x = y;
            y = tmp;
        }
    } while (map[y][x] == '#' || map[y+1][x+1] == '#' || map[y][x] == 'C' || map[y+1][x+1] == 'M');

    map[y][x] = map[y][x+1] = map[y+1][x] = map[y+1][x+1] = symbol;

    return 0;
}

/* This function prints out the map by outputting the character stored
in each pixel, unless it is null, then it outputs a " ". */
int print_map(char map[HEIGHT][WIDTH]) {
    int i, j;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (!map[i][j])
                printf(" ");

            printf("%c", map[i][j]);
        }
        printf("\n");
    }

    return 0;
}