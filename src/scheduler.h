#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"
#include "file.h"

typedef struct Task
{
    int id;
    int time;
} Task;

int run(char* filename, int max);
void process(Task* task);
void task(Queue* taskQueue, File* taskFile);
void usage(char* exe);

#endif
