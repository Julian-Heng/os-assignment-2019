#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"
#include "file.h"

typedef struct Task
{
    int id;
    int time;
    int cpu;
    time_t arrivalTime;
    time_t serviceTime;
    time_t completionTime;
} Task;

typedef struct SharedData
{
    Queue* readyQueue;
    File* taskFile;
    FILE* logFile;
} SharedData;

typedef struct CpuData
{
    SharedData* data;
    int id;
    int numTasks;
    int totalWaitingTime;
    int totalTurnaroundTime;
} CpuData;

#define INT_REAL_DIV(a, b) \
    ((double)(a) / (double)(b))

int run(char* filename, int max);
void* task(void* args);
void* process(void* args);

void taskThreadAddTask(Queue* taskQueue, File* taskFile, FILE* logFile);

void printCpuStat(FILE* f, int id, Task* task);
void printTime(FILE* f, time_t secs);
void usage(char* exe);

#endif
