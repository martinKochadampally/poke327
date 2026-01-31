#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
    queue_item *next;
    char val;
    int coordinates[2];
} queue_item;

typedef struct {
    queue_item *first;
    int size;
} queue;

int queue_init(queue *q);
int queue_enqueue(queue *q, char val, int coordinates[2]);
int queue_dequeue(queue *q, char *val, int coordinates[2]);
int queue_peek(queue *q, char *val, int coordinates[2]);
int queue_size(queue *q, int *size);
int queue_destroy(queue *q);

#endif