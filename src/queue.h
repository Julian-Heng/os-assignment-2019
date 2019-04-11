#ifndef QUEUE_H
#define QUEUE_H

#include "linkedList.h"

typedef struct Queue
{
    LinkedList* link;
    int max;
} Queue;
typedef LinkedListNode QueueNode;

Queue* initQueue(int max);
int enqueue(Queue* queue, void* val, int isMalloc);
QueueNode* dequeue(Queue* queue, void** val, int* isMalloc);
void peek(Queue* queue, void** val, int* isMalloc);
int getQueueLength(Queue* queue);
int getQueueMaxLength(Queue* queue);
int getQueueRemainingCapacity(Queue* queue);
void clearQueue(Queue** queue);
int isQueueEmpty(Queue* queue);
int isQueueFull(Queue* queue);

#endif
