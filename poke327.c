#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "queue.h"
#include "terrain.h"

#define HEIGHT 21
#define WIDTH 80


/*
    % - boulders and mountainous rejions.
    ^ - trees and forests.
    : - long grass.
    . - clearings.
    ~ - water.
    * - artic.
    # - roads.
    Cs - are Pokemon Centers.
    Ms - are Pokemarts.
    @ - Player Character
*/
int main(int argc, char *argv[]) {
    //-----------Variables-------------------------------------
    char map[HEIGHT][WIDTH];
    char characters[] = {'.', '.', '~', ':', '^', '*', ':', '.', '%'};
    int length = sizeof(characters) / sizeof(characters[0]);

    int p1, p2, inter;
    int i;
    int x, y;
    queue q;

    srand(time(NULL));

    //---------------------------------------------------------

    clear_map(map);

    queue_init(&q);

    for (i = 0; i < length; i++) {
        x = 1 + rand()%(WIDTH - 2);
        y = 1 + rand()%(HEIGHT - 2);
        map[y][x] = characters[i];
        if (queue_enqueue(&q, x, y)) {
            return -1;
        }
    }

    //print_map(map);

    if (seed_map(map, &q))
        return -1;
    
    p1 = 2 + rand()%(HEIGHT - 3);
    p2 = 2 + rand()%(HEIGHT - 3);
    inter = WIDTH/4 + rand()%(WIDTH - WIDTH/4);

    place_roads(map, 0, p1, p2, inter);
    place_pokemon_center(map, p1, 0, inter);

    p1 = 2 + rand()%(WIDTH - 3);
    p2 = 2 + rand()%(WIDTH - 3);
    inter = HEIGHT/5 + rand()%(HEIGHT - HEIGHT/5);

    place_roads(map, 1, p1, p2, inter);
    place_pokemart(map, p1, 0, inter);

    init_borders(map);

    print_map(map);

    queue_destroy(&q);

    return 0;
}