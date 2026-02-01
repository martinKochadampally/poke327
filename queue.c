#include <stdio.h>
#include <stdlib.h>

#include "queue.h"


int queue_init(queue *q){
    q->first = NULL;
    q->size = 0;
}

int queue_enqueue(queue *q, int x, int y){ // malloc
    queue_item *tmp;

    if (!(tmp = malloc(sizeof (*tmp)))) {
        return -1;
    }
    return 0;
}

int queue_dequeue(queue *q, int *x, int *y){ // free

    return 0;
}

int queue_peek(queue *q, int *x, int *y){
    return 0;
}

int queue_size(queue *q, int *size){
    *size = q->size;
    return 0;
}

int queue_isEmpty(queue *q){
    if (q->size == 0) {
        return 1;
    }
    return 0;
}

int queue_destroy(queue *q){ // free
    return 0;
}

