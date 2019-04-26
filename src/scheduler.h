#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"
#include "file.h"

/**
 * A struct containing information of a single task
 * These tasks are stored in the readyQueue
 **/
typedef struct Task
{
    /**
     * id             - The task id
     * time           - The task cpu burst time
     * cpu            - The cpu id it is assigned to
     * arrivalTime    - The unix time of arrival
     * serviceTime    - The unix time of being serviced
     * completionTime - The unix time of the task completed
     **/

    int id;
    int time;
    int cpu;
    time_t arrivalTime;
    time_t serviceTime;
    time_t completionTime;
} Task;

/**
 * A struct containing data to be shared within the task thread and the
 * 3 cpu threads
 **/
typedef struct SharedData
{
    /**
     * readyQueue          - The Queue containing all the tasks to be
     *                       serviced
     * taskFile            - The File struct of the task file
     * logFile             - The File struct of the log file
     * numTasks            - The shared stat for total number of tasks
     * totalWaitingTime    - The shared stat for the total waiting time
     * totalTurnaroundTime - The shared stat for the total turnaround time
     *
     * Critical sections:
     *      Locked by queueMutex:
     *          readyQueue
     *
     *      Locked by logMutex:
     *          logFile
     *
     *      Locked by statMutex:
     *          numTasks
     *          totalWaitingTime
     *          totalTurnaroundTime
     *
     * taskFile does not need a mutex lock because only the task thread
     * interacts with it
     **/

    Queue* readyQueue;
    File* taskFile;
    File* logFile;

    int* numTasks;
    int* totalWaitingTime;
    int* totalTurnaroundTime;
} SharedData;

/**
 * A struct for cpu threads, which is essentially a wrapper for a
 * SharedData struct, but has an int for the cpu id
 **/
typedef struct CpuData
{
    SharedData* data;
    int id;
} CpuData;

#define INT_REAL_DIV(a, b) \
    ((double)(a) / (double)(b))

/* Function prototypes */
int run(char* filename, int max);
void* task(void* args);
void* cpu(void* args);
void taskThreadAddTask(Queue* readyQueue, File* taskFile, File* logFile);
void logCpuStat(File* logFile, int id, Task* task);
void strTime(char** str, time_t secs);
void logger(File* file, char* format, ...);
int sanitizeTaskFile(File* file);
void usage(char* exe);

#endif /* End of scheduler.h */
