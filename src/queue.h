#ifndef QUEUE_H
#define QUEUE_H

#include "linkedList.h"

/**
 * Queue struct that contains a LinkedList and an integer for the
 * maximum amount of nodes allowed for the queue
 **/
typedef struct Queue
{
    LinkedList* link;
    int max;
} Queue;

/* A QueueNode is the same as a LinkedLIstNode */
typedef LinkedListNode QueueNode;

/* Function prototypes */
Queue* initQueue(int max);
int enqueue(Queue* queue, void* val, int isMalloc);
QueueNode* dequeue(Queue* queue, void** val, int* isMalloc);
void clearQueue(Queue** queue);

/* Macros, since queue is a wrapper for a linked list */
#define GET_QUEUE_LEN(queue)            (queue)->link->length
#define GET_QUEUE_MAX_LEN(queue)         (queue)->max
#define GET_QUEUE_REMAIN_CAPACITY(queue) (queue)->max - (queue)->link->length
#define IS_QUEUE_EMPTY(queue)              IS_LIST_EMPTY((queue)->link)
#define IS_QUEUE_FULL(queue)               queue->max == queue->link->length

#endif /* End of queue.h */
