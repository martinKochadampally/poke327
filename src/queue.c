#include <stdio.h>
#include <stdlib.h>

#include "queue.h"


int queue_init(queue *q){
    q->first = NULL;
    q->size = 0;
}

int queue_enqueue(queue *q, char val){ // malloc
    queue_item *tmp;

    if (!(tmp = malloc(sizeof (*tmp)))) {
        return -1;
    }
    return 0;
}

int queue_dequeue(queue *q, char *val){ // free

    return 0;
}

int queue_peek(queue *q, char *val){
    *val = q->first;
    return 0;
}

int queue_size(queue *q, int *size){
    *size = q->size;
    return 0;
}

int queue_destroy(queue *q){ // free
    return 0;
}

