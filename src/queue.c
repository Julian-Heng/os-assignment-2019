/**
 * Filename: queue.c
 * Author:   Julian Heng (19473701)
 * Purpose:  A queue data structure, which serves as a wrapper for a
 *           linked list
 **/

#include <stdlib.h>

#include "linkedList.h"
#include "queue.h"

#define FALSE 0
#define TRUE !FALSE

/**
 * Return a Queue, which is typedef to a LinkedList
 **/
Queue* initQueue(int max)
{
    Queue* queue;

    if ((queue = (Queue*)malloc(sizeof(Queue))))
    {
        queue->link = initList();
        queue->max = max;
    }

    return queue;
}

/**
 * Add a void pointer to the queue
 **/
int enqueue(Queue* queue, void* val, int isMalloc)
{
    int ret = FALSE;
    if (queue->link->length < queue->max)
    {
        insertLast(queue->link, val, isMalloc);
        ret = TRUE;
    }

    return ret;
}

/**
 * Removes from the queue. Garbage collection needs to be handled by the
 * calling function
 **/
QueueNode* dequeue(Queue* queue, void** val, int* isMalloc)
{
    return removeFirst(queue->link, val, isMalloc);
}

void peek(Queue* queue, void** val, int* isMalloc)
{
    peekFirst(queue->link, val, isMalloc);
}

int getQueueLength(Queue* queue)
{
    return queue->link->length;
}

int getQueueMaxLength(Queue* queue)
{
    return queue->max;
}

int getQueueRemainingCapacity(Queue* queue)
{
    return queue->max - queue->link->length;
}

void clearQueue(Queue** queue)
{
    clearList(&((*queue)->link));
    free(*queue);
    *queue = NULL;
}

int isQueueEmpty(Queue* queue)
{
    return isListEmpty(queue->link);
}

int isQueueFull(Queue* queue)
{
    return queue->max == queue->link->length;
}
