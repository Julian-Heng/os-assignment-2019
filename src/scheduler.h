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
    File* logFile;

    int* numTasks;
    int* totalWaitingTime;
    int* totalTurnaroundTime;
} SharedData;

typedef struct CpuData
{
    SharedData* data;
    int id;
} CpuData;

#define INT_REAL_DIV(a, b) \
    ((double)(a) / (double)(b))

int run(char* filename, int max);
void* task(void* args);
void* cpu(void* args);

void taskThreadAddTask(Queue* taskQueue, File* taskFile, File* logFile);

void printCpuStat(File* logFile, int id, Task* task);
void strTime(char** str, time_t secs);
void logger(File* file, char* format, ...);
void usage(char* exe);

#endif
