#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "queue.h"

#define HEIGHT 21
#define WIDTH 80

int seed_map(char map[HEIGHT][WIDTH], queue *q);

/*
    % - boulders and roads.
    ^ - trees and forests.
    : - long grass.
    . - clearings.
    ~ - water.
    # - roads.
    Cs - are Pokemon Centers.
    Ms - are Pokemarts.
    @ - Player Character
*/
int main(int argc, char *argv[]) {
    int i, x, y;
    char map[HEIGHT][WIDTH];
    queue q;
    
    srand(time(NULL));

    char characters[] = {'.', ':', '^', '~', '%'};
    int length = sizeof(characters) / sizeof(characters[0]);
    queue_init(&q);

    for (i = 0; i < length; i++) {
        x = 1 + rand()%79;
        y = 1 + rand()%20;
        map[y][x] = characters[i];
        if (queue_enqueue(&q, x, y)) {
            return -1;
        }
    }

    printf("%c\n", map[0][0]);
    if (seed_map(map, &q))
        return -1;
    printf("%c\n", map[0][0]);

    queue_destroy(&q);

    return 0;
}


int seed_map(char map[HEIGHT][WIDTH], queue *q) {
    int x, y;

    while (!queue_isEmpty(&q)) {
        queue_dequeue(&q, &x, &y);

        if (y-1 > -1 && x-1 > -1 && !map[y-1][x-1]){
            map[y-1][x-1] = map[x,y];
            if (queue_enqueue(&q, x-1, y-1)) {
                return -1;
            }
        }
        if (y-1 > -1 && !map[y-1][x]){
            map[y-1][x] = map[x,y];
            if (queue_enqueue(&q, x, y-1)) {
                return -1;
            }
        }
        if (y-1 > -1 && x+1 < WIDTH && !map[y-1][x+1]){
            map[y-1][x+1] = map[x,y];
            if (queue_enqueue(&q, x+1, y-1)) {
                return -1;
            }
        }
        if (x+1 < WIDTH && !map[y][x+1]){
            map[y][x+1] = map[x,y];
            if (queue_enqueue(&q, x+1, y)) {
                return -1;
            }
        }
        if (y+1 < HEIGHT && x+1 < WIDTH && !map[y+1][x+1]){
            map[y+1][x+1] = map[x,y];
            if (queue_enqueue(&q, x+1, y+1)) {
                return -1;
            }
        }
        if (y+1 < HEIGHT && x && !map[y+1][x]){
            map[y+1][x] = map[x,y];
            if (queue_enqueue(&q, x, y+1)) {
                return -1;
            }

        }
        if (y+1 < HEIGHT && x-1 > -1 && !map[y+1][x-1]){
            map[y+1][x-1] = map[x,y];
            if (queue_enqueue(&q, x-1, y+1)) {
                return -1;
            }
        }
        if (y && x-1 > -1 && !map[y][x-1]){
            map[y][x-1] = map[x,y];
            if (queue_enqueue(&q, x-1, y)) {
                return -1;
            }
        }
    }

    return 0;
}