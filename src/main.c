#include <stdlib.h>
#include <time.h>

#define HEIGHT 21
#define WIDTH 80

int intitialize_map(char *map[HEIGHT][WIDTH]);

int main(int argc, char *argv[]) {
    char *map[HEIGHT][WIDTH];

    srand(time(NULL));

    if (!initialize_map(&map))
        return -1;
    return 0;
}

int intitialize_map(char *map[HEIGHT][WIDTH]) {
    return 0;
}