#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

/* Initializes the queue and pointers to null. */
int queue_init(queue *q){
    q->first = NULL;
    q->last = NULL;
    q->size = 0;
    return 0;
}

/* Creates a new queue item using malloc and given x and y coordinates
and then adds it to the back of queue by adjusting the current last item's 
pointer and the last pointer of the queue. The new elements next 
pointer is also set to null. 

If malloc fails return -1 */
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

/* Stores the current first element in a tmp variable, then
sets the new first as the next element in the queue, decrements
size and frees the tmp so there is no memory leak.

If queue is null or size is 0, returns -1. */
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

/* Stores the coordinates of the item at the top of
the queue in the given addresses. 

If queue is null or size is 0, returns -1. */
int queue_peek(queue *q, int *x, int *y){
    if (!q || !q->size) {
        return -1;
    }

    *x = q->first->x;
    *y = q->first->y;
    return 0;
}

/* Stores the number of elements waiting in queue in
the specified address.

If queue is null returns -1. */
int queue_size(queue *q, int *size){
    if (!q) {
        return -1;
    }

    *size = q->size;
    return 0;
}

/* Removes all the elements in queue, and frees
them as it goes. Lastly sets queue size to 0. */
int queue_destroy(queue *q){
    queue_item *tmp; 

    q->last = NULL;

    while ((tmp = q->first)) {
        q->first = tmp->next;
        free(tmp);
    }

    q->size = 0;

    q = NULL;

    return 0;
}

