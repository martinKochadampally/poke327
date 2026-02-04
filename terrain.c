#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "queue.h"

#define HEIGHT 21
#define WIDTH 80


int clear_map(char map[HEIGHT][WIDTH]) {
    int i, j;

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            map[i][j] = '\0';
        }
    }

    return 0;
}

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


int seed_map(char map[HEIGHT][WIDTH], queue *q) {
    int x, y, size;
    // int count1 = 0;

    while (!queue_size(q, &size) && size) {
        queue_dequeue(q, &x, &y);
        // if (map[y][x] == '%' && ++count1 > 100) {
        //     continue;
        // }

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

int place_pokemon_center(char map[HEIGHT][WIDTH],int road, int p1, int p2){
    int x = p1 + 1 + rand()%(p2-p1-2);
    int y;

    if (road > HEIGHT/4)
        y = road - 2;
    else
        y = road + 1;

    map[y][x] = 'C';
    map[y][x+1] = 'C';
    map[y+1][x] = 'C';
    map[y+1][x+1] = 'C';

    return 0;
}

int place_pokemart(char map[HEIGHT][WIDTH],int road, int p1, int p2){
    int y = p1 + 1 + rand()%(p2-p1-2);
    int x;

    if (road > WIDTH/5)
        x = road - 2;
    else
        x = road + 1;

    map[y][x] = 'M';
    map[y][x+1] = 'M';
    map[y+1][x] = 'M';
    map[y+1][x+1] = 'M';

    return 0;
}


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