#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "queue.h"
#include "map.h"


/*
    % - immovable boulders and mountainous regions.
    ^ - forest.
    : - long grass.
    . - clearings.
    ~ - lake.
    * - artic.
    # - roads.
    Cs - are Pokemon Centers.
    Ms - are Pokemarts.
    @ - Player Character
*/
int main(int argc, char *argv[]) {
    world *w;
    queue visited;
    int y, x;
    char input[20];
    int new_x_pos, new_y_pos;

    if (!( w = malloc(sizeof(world)))) return 1;
    if (world_init(&w)) return -1;

    queue_init(&visited);

    srand(time(NULL));
    
    input[0] = 'f';
    new_x_pos = 0;
    new_y_pos = 0;
    
    while (input[0] != 'q') {
        if (input[0] == 'f' ) {
            x = new_x_pos + 200;
            y = new_y_pos + 200;
        } 
        else if (input[0] == 'n' && y > 0) y--;
        else if (input[0] == 's' && y < 400) y++;
        else if (input[0] == 'e' && x < 400) x++;
        else if (input[0] == 'w' && x > 0) x--;
        else {
            printf("Invalid command or edge of w reached!\n");
        }

        if (!w->maps[y][x]) {
            w->maps[y][x] = malloc(sizeof (map));

            if (!w->maps[y][x]) {
                fprintf(stderr, "Malloc failed.");
                return -1;
            }

            if (queue_enqueue(&visited, x, y)) {
                return -1;
            }

            init_map(w->maps[y][x], x, y);

            // Check north.
            if (y > 0 && w->maps[y - 1][x] != NULL) {
                w->maps[y][x]->pN = w->maps[y - 1][x]->pS;
            } else {
                w->maps[y][x]->pN = 4 + rand() % (WIDTH - 8);
            }

            // Check south.
            if (y < 400 && w->maps[y + 1][x] != NULL) {
                w->maps[y][x]->pS = w->maps[y + 1][x]->pN;
            } else {
                w->maps[y][x]->pS = 4 + rand() % (WIDTH - 8);
            }

            // Check west.
            if (x > 0 && w->maps[y][x - 1] != NULL) {
                w->maps[y][x]->pW = w->maps[y][x - 1]->pE;
            } else {
                w->maps[y][x]->pW = 4 + rand() % (HEIGHT - 8);
            }

            // Check East.
            if (x < 400 && w->maps[y][x + 1] != NULL) {
                w->maps[y][x]->pE = w->maps[y][x + 1]->pW;
            } else {
                w->maps[y][x]->pE = 4 + rand() % (HEIGHT - 8);
            }

            if (seed_map(w->maps[y][x])) {
                fprintf(stderr, "Seeding failed.");
                return -1;
            }
            
            place_paths_and_buildings(w->maps[y][x]);

        }

        print_map(w->maps[y][x]);

        printf("Enter a command (n, s, e, w, f <x> <y>, q): ");

        if (!fgets(input, sizeof (input), stdin)) {
            return -1;
        }

        if (input[0] == 'f') {
            int temp_x, temp_y;
            if (sscanf(input, "f %d %d", &temp_x, &temp_y) == 2) {
                if (temp_x >= -200 && temp_x <= 200 && temp_y >= -200 && temp_y <= 200) {
                    new_x_pos = temp_x;
                    new_y_pos = temp_y;
                } else {
                    printf("Out of bounds! Stay between -200 and 200.\n");
                }
            }
        }
    }

    while (!queue_dequeue(&visited, &x, &y)) {
        free(w->maps[y][x]);
    }

    queue_destroy(&visited);

    world_destoy(&w);

    free(w);

    return 0;
}