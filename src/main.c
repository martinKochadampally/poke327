#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT 21
#define WIDTH 80

int initialize_map(char map[HEIGHT][WIDTH]);

int main(int argc, char *argv[]) {
    char map[HEIGHT][WIDTH];

    srand(time(NULL));
    printf("%c\n", map[0][0]);
    if (initialize_map(map))
        return -1;
    printf("%c\n", map[0][0]);
    return 0;
}

int initialize_map(char map[HEIGHT][WIDTH]) {
    map[0][0] = 'Z';
    return 0;
}