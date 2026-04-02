#ifndef HEAP_H
#define HEAP_H

/* This is my implementation of a fibbonacci heap. */

typedef struct heap_item
{
    void *t;
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
heap_item* heap_insert(heap *h, void *t, int key);
int heap_min(heap *h, void **t);
int heap_extract_min(heap *h, void **t, int *cost);
int decrease_key(heap *h, heap_item *node, int new_key);
int heap_size(heap *h, int *size);
void heap_destroy(heap *h); 


#endif