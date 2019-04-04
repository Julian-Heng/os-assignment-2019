/**
 * Filename: queue.c
 * Author:   Julian Heng (19473701)
 * Purpose:  A queue data structure, which serves as a wrapper for a linked
 *           list
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
    Queue* q;

    if ((q = (Queue*)malloc(sizeof(Queue))))
    {
        q -> queue = initList();
        q -> max = max;
    }

    return q;
}

/**
 * Add a void pointer to the queue
 **/
int enqueue(Queue* q, void* v, int isMalloc)
{
    int ret = FALSE;
    if (getQueueLength(q) < q -> max)
    {
        insertLast(q -> queue, v, isMalloc);
        ret = TRUE;
    }

    return ret;
}

/**
 * Removes from the queue. Garbage collection needs to be handled by the
 * calling function
 **/
QueueNode* dequeue(Queue* q, void** v, int* i)
{
    return removeFirst(q -> queue, v, i);
}

void peek(Queue* q, void** v, int* i)
{
    peekFirst(q -> queue, v, i);
}

int getQueueLength(Queue* q)
{
    return q -> queue -> length;
}

int getQueueMaxLength(Queue* q)
{
    return q -> max;
}

void clearQueue(Queue** q)
{
    clearList(&((*q) -> queue));
    free(*q);
    *q = NULL;
}

int isQueueEmpty(Queue* q)
{
    return isListEmpty(q -> queue);
}
