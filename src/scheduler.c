#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "scheduler.h"
#include "file.h"
#include "queue.h"

#define FALSE 0
#define TRUE !FALSE

#define NUM_THREADS 3

#define TIME                "%02d:%02d:%02d"
#define TASK_NUM            "Number of tasks: %d\n"
#define TASK_TALLY          "Number of tasks put into Ready-Queue: %d\n"
#define TASK_TERMINATE      "Terminate at time: "

#define CPU_HEAD            "Statistics for CPU-%d\n"
#define CPU_TERMINATE       "CPU-%d terminates after servicing %d tasks\n"

#define RESULT_AVG_WAIT     "Average waiting time: %.2f secs\n"
#define RESULT_AVG_TURN     "Average turnaround time: %.2f secs\n"

#define LOG_FILE            "simulation_log"

pthread_mutex_t queueMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t logMutex    = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t queueFull    = PTHREAD_COND_INITIALIZER;
pthread_cond_t queueEmpty   = PTHREAD_COND_INITIALIZER;

int main(int argc, char** argv)
{
    int ret;
    int max;

    if (argc < 3)
    {
        usage(argv[0]);
        ret = 1;
    }
    else
    {
        max = atoi(argv[2]);

        if (max > 1 && 11 > max)
        {
            ret = run(argv[1], max);
        }
        else
        {
            usage(argv[0]);
            ret = 2;
        }
    }

    return ret;
}

int run(char* filename, int max)
{
    pthread_t taskThread;
    pthread_t cpuThread[3];
    SharedData sharedData;
    CpuData cpuData[3];

    Queue* readyQueue;
    File* taskFile;

    int i;
    int ret;

    int totalTasks;
    int totalWaitingTime;
    int totalTurnaroundTime;

    totalTasks = 0;
    totalWaitingTime = 0;
    totalTurnaroundTime = 0;

    taskFile = (File*)malloc(sizeof(File));
    readyQueue = initQueue(max);
    readFile(filename, taskFile);

    sharedData.readyQueue = readyQueue;
    sharedData.taskFile = taskFile;
#ifdef DEBUG
    sharedData.logFile = stderr;
#else
    sharedData.logFile = fopen(LOG_FILE, "a");
#endif
    fprintf(sharedData.logFile, "---\n");

    for (i = 0; i < NUM_THREADS; i++)
    {
        cpuData[i].data = &sharedData;
        cpuData[i].id = i + 1;
        cpuData[i].numTasks = 0;
        cpuData[i].totalWaitingTime = 0;
        cpuData[i].totalTurnaroundTime = 0;
    }

    pthread_create(&taskThread, NULL, task, &sharedData);
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&cpuThread[i], NULL, process, &cpuData[i]);
    }

    pthread_join(taskThread, NULL);
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(cpuThread[i], NULL);
    }

    for (i = 0; i < NUM_THREADS; i++)
    {
        totalTasks += cpuData[i].numTasks;
        totalWaitingTime += cpuData[i].totalWaitingTime;
        totalTurnaroundTime += cpuData[i].totalTurnaroundTime;
    }

    fprintf(sharedData.logFile, TASK_NUM, totalTasks);
    fprintf(sharedData.logFile, RESULT_AVG_WAIT,
                                INT_REAL_DIV(totalWaitingTime,
                                             totalTasks));
    fprintf(sharedData.logFile, RESULT_AVG_TURN,
                                INT_REAL_DIV(totalTurnaroundTime,
                                             totalTasks));

    clearQueue(&readyQueue);
    clearQueue(&(taskFile->data));

    free(taskFile);
    taskFile = NULL;
    fclose(sharedData.logFile);

    ret = 0;
    return ret;
}

void* task(void* args)
{
    SharedData* sharedData = (SharedData*)args;
    Queue* readyQueue = sharedData->readyQueue;
    File* taskFile = sharedData->taskFile;
    FILE* logFile = sharedData->logFile;

    int numTasks = 0;
    time_t rawSecs;

    while (TRUE)
    {
        pthread_mutex_lock(&queueMutex);

        if (isQueueEmpty(taskFile->data))
        {
            pthread_mutex_unlock(&queueMutex);
            time(&rawSecs);

            pthread_mutex_lock(&logMutex);
            fprintf(logFile, TASK_TALLY, numTasks);
            fprintf(logFile, TASK_TERMINATE);
            printTime(logFile, rawSecs);
            fprintf(logFile, "\n\n");
            pthread_mutex_unlock(&logMutex);

            pthread_exit(NULL);
        }

        if (getQueueRemainingCapacity(readyQueue) < 2)
        {
            /* readyQueue is full */
            pthread_cond_signal(&queueFull);

            while (getQueueRemainingCapacity(readyQueue) < 2)
            {
                pthread_cond_wait(&queueEmpty, &queueMutex);
            }
        }

        while (getQueueRemainingCapacity(readyQueue) > 1 &&
               ! isQueueEmpty(taskFile->data))
        {
            if (! isQueueEmpty(taskFile->data))
            {
                taskThreadAddTask(readyQueue, taskFile, logFile);
                numTasks++;
            }

            if (! isQueueEmpty(taskFile->data))
            {
                taskThreadAddTask(readyQueue, taskFile, logFile);
                numTasks++;
            }
        }

        pthread_cond_signal(&queueFull);
        pthread_mutex_unlock(&queueMutex);
    }
}

void* process(void* args)
{
    CpuData* cpuData = (CpuData*)args;
    SharedData* sharedData = cpuData->data;
    int id = cpuData->id;

    Queue* readyQueue = sharedData->readyQueue;
    File* taskFile = sharedData->taskFile;
    FILE* logFile = sharedData->logFile;

    QueueNode* node;
    Task* task;
    int isMalloc;

    while (TRUE)
    {
        pthread_mutex_lock(&queueMutex);

        if (isQueueEmpty(taskFile->data) && isQueueEmpty(readyQueue))
        {
            pthread_mutex_unlock(&queueMutex);
            pthread_mutex_lock(&logMutex);
            fprintf(logFile, CPU_TERMINATE, id, cpuData->numTasks);
            pthread_mutex_unlock(&logMutex);

            pthread_exit(NULL);
        }

        while (isQueueEmpty(readyQueue))
        {
            /* readyQueue is empty */
            pthread_cond_signal(&queueEmpty);

            while (isQueueEmpty(readyQueue))
            {
                pthread_cond_wait(&queueFull, &queueMutex);
            }
        }

        node = dequeue(readyQueue, (void**)&task, &isMalloc);

        pthread_mutex_unlock(&queueMutex);

        time(&(task->serviceTime));
        (cpuData->numTasks)++;
        (cpuData->totalWaitingTime) += (task->serviceTime) -
                                       (task->arrivalTime);

        pthread_mutex_lock(&logMutex);
        printCpuStat(logFile, id, task);
        fprintf(logFile, "Service Time: ");
        printTime(logFile, task->serviceTime);
        fprintf(logFile, "\n\n");
        pthread_mutex_unlock(&logMutex);

        sleep(task->time);
        time(&(task->completionTime));
        (cpuData->totalTurnaroundTime) += (task->completionTime) -
                                          (task->arrivalTime);

        pthread_mutex_lock(&logMutex);
        printCpuStat(logFile, id, task);
        fprintf(logFile, "Completion Time: ");
        printTime(logFile, task->completionTime);
        fprintf(logFile, "\n\n");
        pthread_mutex_unlock(&logMutex);

        free(task);
        task = NULL;

        free(node);
        node = NULL;
    }
}

void taskThreadAddTask(Queue* taskQueue, File* taskList, FILE* logFile)
{
    Task* taskNode;
    QueueNode* node;

    char* str;
    int taskId;
    int cpuBurst;
    int isMalloc;

    taskNode = (Task*)malloc(sizeof(Task));
    node = dequeue(taskList->data, (void**)&str, &isMalloc);

    sscanf(str, "task%d %d", &taskId, &cpuBurst);
    taskNode->id = taskId;
    taskNode->time = cpuBurst;
    taskNode->cpu = INT_MAX;
    time(&(taskNode->arrivalTime));
    enqueue(taskQueue, taskNode, isMalloc);

    pthread_mutex_lock(&logMutex);
    fprintf(logFile, "task%d: %d\n", taskId, cpuBurst);
    fprintf(logFile, "Arrival time: ");
    printTime(logFile, taskNode->arrivalTime);
    fprintf(logFile, "\n\n");
    pthread_mutex_unlock(&logMutex);

    free(str);
    free(node);

    str = NULL;
    node = NULL;
}

void printCpuStat(FILE* f, int id, Task* task)
{
    fprintf(f, CPU_HEAD, id);
    fprintf(f, "Task %d\n", task->id);
    fprintf(f, "Arrival Time: ");
    printTime(f, task->arrivalTime);
    fprintf(f, "\n");
}

void printTime(FILE* f, time_t secs)
{
    struct tm* timeinfo = localtime(&secs);
    fprintf(f, TIME, timeinfo->tm_hour,
                     timeinfo->tm_min,
                     timeinfo->tm_sec);
}

void usage(char* exe)
{
    fprintf(stderr, "Usage: %s [task-file] [2-10]\n", exe);
}
