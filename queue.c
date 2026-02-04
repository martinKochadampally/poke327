#include <stdio.h>
#include <stdlib.h>

#include "queue.h"


int queue_init(queue *q){
    q->first = NULL;
    q->last = NULL;
    q->size = 0;
    return 0;
}

int queue_enqueue(queue *q, int x, int y){
    queue_item *tmp;

    if (!(tmp = malloc(sizeof (*tmp)))) { // If malloc fails.
        return -1;
    }

    tmp->x = x;
    tmp->y = y;
    tmp->next = NULL;

    if (!q->size) {
        q->first = tmp;
        q->last = tmp;
    }
    else {
        q->last->next = tmp;
        q->last = tmp;
    }

    q->size++;

    return 0;
}

int queue_dequeue(queue *q, int *x, int *y){ // free
    queue_item *tmp;

    if (!q || !q->size) {
        return -1;
    }

    tmp = q->first;
    q->first = tmp->next;

    *x = tmp->x;
    *y = tmp->y;

    free(tmp);

    q->size--;

    if (q->size == 0) {
        q->last = NULL;
    }

    return 0;
}

int queue_peek(queue *q, int *x, int *y){
    if (!q || !q->size) {
        return -1;
    }

    *x = q->first->x;
    *y = q->first->y;
    return 0;
}

int queue_size(queue *q, int *size){
    *size = q->size;
    return 0;
}

int queue_destroy(queue *q){
    queue_item *tmp; 

    q->last = NULL;

    while ((tmp = q->first)) {
        q->first = tmp->next;
        free(tmp);
    }

    q->size = 0;

    return 0;
}

