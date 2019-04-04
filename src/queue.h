#ifndef QUEUE_H
#define QUEUE_H

typedef LinkedList Queue;
typedef LinkedListNode QueueNode;

Queue* initQueue(void);
void enqueue(Queue* q, void* v, int isMalloc);
QueueNode* dequeue(Queue* q, void** v, int* i);
void peek(Queue* q, void** v, int* i);
int getQueueLength(Queue* q);
void clearQueue(Queue** q);
int isQueueEmpty(Queue* q);

#endif
