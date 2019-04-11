#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"
#include "file.h"

typedef struct Task
{
    int id;
    int time;
} Task;

typedef struct SharedData
{
    Queue* readyQueue;
    File* taskFile;
} SharedData;

int run(char* filename, int max);
/*
void process(Task* task);
void task(Queue* taskQueue, File* taskFile);
*/
void* task(void* args);
void* process(void* args);

void taskThreadAddTask(Queue* taskQueue, File* taskFile);
void cpuThreadRunTask(Task* task);

void usage(char* exe);

#endif
