/**
 * Filename: queue.c
 * Author:   Julian Heng (19473701)
 * Purpose:  A queue data structure, which serves as a wrapper for a linked
 *           list
 **/

#include "linkedList.h"
#include "queue.h"

/**
 * Return a Queue, which is typedef to a LinkedList
 **/
Queue* initQueue(void)
{
    return initList();
}

/**
 * Add a void pointer to the queue
 **/
void enqueue(Queue* q, void* v, int isMalloc)
{
    insertLast(q, v, isMalloc);
}

/**
 * Removes from the queue. Garbage collection needs to be handled by the
 * calling function
 **/
QueueNode* dequeue(Queue* q, void** v, int* i)
{
    return removeFirst(q, v, i);
}

void peek(Queue* q, void** v, int* i)
{
    peekFirst(q, v, i);
}

int getQueueLength(Queue* q)
{
    return q -> length;
}

void clearQueue(Queue** q)
{
    clearList(q);
}

int isQueueEmpty(Queue* q)
{
    return isListEmpty(q);
}
