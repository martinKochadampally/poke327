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
    map *world[401][401] = {{NULL}};
    queue visited;
    int y, x;
    char input[20];
    int new_x_pos, new_y_pos;

    srand(time(NULL));
    queue_init(&visited);
    
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
            printf("Invalid command or edge of world reached!\n");
        }

        if (!world[y][x]) {
            world[y][x] = malloc(sizeof (map));

            if (!world[y][x]) {
                fprintf(stderr, "Malloc failed.");
                return -1;
            }

            if (queue_enqueue(&visited, x, y)) {
                return -1;
            }

            init_map(world[y][x], x, y);

            // Check north.
            if (y > 0 && world[y - 1][x] != NULL) {
                world[y][x]->pN = world[y - 1][x]->pS;
            } else {
                world[y][x]->pN = 4 + rand() % (WIDTH - 8);
            }

            // Check south.
            if (y < 400 && world[y + 1][x] != NULL) {
                world[y][x]->pS = world[y + 1][x]->pN;
            } else {
                world[y][x]->pS = 4 + rand() % (WIDTH - 8);
            }

            // Check west.
            if (x > 0 && world[y][x - 1] != NULL) {
                world[y][x]->pW = world[y][x - 1]->pE;
            } else {
                world[y][x]->pW = 4 + rand() % (HEIGHT - 8);
            }

            // Check East.
            if (x < 400 && world[y][x + 1] != NULL) {
                world[y][x]->pE = world[y][x + 1]->pW;
            } else {
                world[y][x]->pE = 4 + rand() % (HEIGHT - 8);
            }

            if (seed_map(world[y][x])) {
                fprintf(stderr, "Seeding failed.");
                return -1;
            }
            
            place_paths_and_buildings(world[y][x]);

        }

        print_map(world[y][x]);

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
        free(world[y][x]);
    }

    return 0;
}