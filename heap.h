#ifndef HEAP_H
#define HEAP_H

#include "map.h"

/* This is my implementation of a fibbonacci heap. */

typedef struct heap_item
{
    terrain *t;
    struct heap_item *left, *right, *children, *parent;
    int degree;
    int key;
    int mark;
} heap_item;

typedef struct
{
    heap_item *min;
    int num_elements; 
} heap;

void heap_init(heap *h);
int heap_insert(heap *h, terrain *t, int key);
int heap_min(heap *h, terrain **t);
int heap_extract_min(heap *h, terrain **t, int *cost);
int decrease_key(heap *h, terrain *t, int new_key);
int is_in_heap(heap *h, terrain *t);
int heap_size(heap *h, int *size);
void heap_destroy(heap *h); 


#endif