#ifndef QUEUE_H
#define QUEUE_H

#include "linkedList.h"

typedef struct Queue
{
    LinkedList* queue;
    int max;
} Queue;
typedef LinkedListNode QueueNode;

Queue* initQueue(int max);
int enqueue(Queue* q, void* v, int isMalloc);
QueueNode* dequeue(Queue* q, void** v, int* i);
void peek(Queue* q, void** v, int* i);
int getQueueLength(Queue* q);
int getQueueMaxLength(Queue* q);
void clearQueue(Queue** q);
int isQueueEmpty(Queue* q);
int isQueueFull(Queue* q);

#endif
