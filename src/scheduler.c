#include <limits.h>
#include <pthread.h>
#include <stdarg.h>
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
#define TASK_TERMINATE      "Terminate at time: %s\n"

#define CPU_HEAD            "Statistics for CPU-%d\n"
#define CPU_TERMINATE       "CPU-%d terminates after servicing %d tasks\n\n"

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
    File* logFile;

    int i;
    int ret;

    int totalTasks;
    int totalWaitingTime;
    int totalTurnaroundTime;

    totalTasks = 0;
    totalWaitingTime = 0;
    totalTurnaroundTime = 0;

    readyQueue = initQueue(max);

    taskFile = initFile(INT_MAX);
    setFilename(filename, taskFile);
    readFile(taskFile);

    logFile = initFile(INT_MAX);
    setFilename(LOG_FILE, logFile);

    sharedData.readyQueue = readyQueue;
    sharedData.taskFile = taskFile;
    sharedData.logFile = logFile;

    logToFile(sharedData.logFile, "---\n");

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
        pthread_create(&cpuThread[i], NULL, cpu, &cpuData[i]);
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

    logToFile(logFile, TASK_NUM, totalTasks);
    logToFile(logFile, RESULT_AVG_WAIT, INT_REAL_DIV(totalWaitingTime,
                                                     totalTasks));
    logToFile(logFile, RESULT_AVG_TURN, INT_REAL_DIV(totalTurnaroundTime,
                                                     totalTasks));

    clearQueue(&readyQueue);

    writeFile(sharedData.logFile, "a");
    freeFile(&(sharedData.logFile));
    freeFile(&(sharedData.taskFile));

    ret = 0;
    return ret;
}

void* task(void* args)
{
    SharedData* sharedData = (SharedData*)args;
    Queue* readyQueue = sharedData->readyQueue;
    File* taskFile = sharedData->taskFile;
    File* logFile = sharedData->logFile;

    char* timeStr;
    int numTasks;
    time_t rawSecs;

    numTasks = 0;

    while (TRUE)
    {
        pthread_mutex_lock(&queueMutex);

        if (isQueueEmpty(taskFile->data))
        {
            pthread_mutex_unlock(&queueMutex);
            time(&rawSecs);

            pthread_mutex_lock(&logMutex);

            strTime(&timeStr, rawSecs);
            logToFile(logFile, TASK_TALLY, numTasks);
            logToFile(logFile, TASK_TERMINATE, timeStr);
            logToFile(logFile, "\n");
            free(timeStr);
            timeStr = NULL;

            pthread_mutex_unlock(&logMutex);

            pthread_exit(NULL);
        }

        if (getQueueRemainingCapacity(readyQueue) < 2)
        {
            /* readyQueue is full */
            pthread_cond_broadcast(&queueFull);

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
                pthread_cond_broadcast(&queueFull);
                numTasks++;
            }

            if (! isQueueEmpty(taskFile->data))
            {
                taskThreadAddTask(readyQueue, taskFile, logFile);
                pthread_cond_broadcast(&queueFull);
                numTasks++;
            }
        }

        pthread_cond_signal(&queueFull);
        pthread_mutex_unlock(&queueMutex);
    }
}

void* cpu(void* args)
{
    CpuData* cpuData = (CpuData*)args;
    SharedData* sharedData = cpuData->data;
    int id = cpuData->id;

    Queue* readyQueue = sharedData->readyQueue;
    File* taskFile = sharedData->taskFile;
    File* logFile = sharedData->logFile;

    QueueNode* node;
    Task* task;
    int isMalloc;
    char* timeStr;

    while (TRUE)
    {
        pthread_mutex_lock(&queueMutex);

        if (isQueueEmpty(taskFile->data) && isQueueEmpty(readyQueue))
        {
            pthread_mutex_unlock(&queueMutex);
            pthread_mutex_lock(&logMutex);
            logToFile(logFile, CPU_TERMINATE, id, cpuData->numTasks);
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

        strTime(&timeStr, task->serviceTime);
        printCpuStat(logFile, id, task);
        logToFile(logFile, "Service Time: %s\n", timeStr);
        logToFile(logFile, "\n");
        free(timeStr);
        timeStr = NULL;

        pthread_mutex_unlock(&logMutex);

        sleep(task->time);
        time(&(task->completionTime));
        (cpuData->totalTurnaroundTime) += (task->completionTime) -
                                          (task->arrivalTime);

        pthread_mutex_lock(&logMutex);

        strTime(&timeStr, task->completionTime);
        printCpuStat(logFile, id, task);
        logToFile(logFile, "Completion Time: %s\n", timeStr);
        logToFile(logFile, "\n");
        free(timeStr);
        timeStr = NULL;

        pthread_mutex_unlock(&logMutex);

        free(task);
        task = NULL;

        free(node);
        node = NULL;
    }
}

void taskThreadAddTask(Queue* taskQueue, File* taskList, File* logFile)
{
    Task* taskNode;
    QueueNode* node;

    char* timeStr;
    char* str;
    int taskId;
    int cpuBurst;
    int isMalloc;

    taskNode = (Task*)malloc(sizeof(Task));
    node = dequeue(taskList->data, (void**)&str, &isMalloc);

    sscanf(str, "task%d %d\n", &taskId, &cpuBurst);
    taskNode->id = taskId;
    taskNode->time = cpuBurst;
    taskNode->cpu = INT_MAX;
    time(&(taskNode->arrivalTime));
    enqueue(taskQueue, taskNode, isMalloc);

    pthread_mutex_lock(&logMutex);
    strTime(&timeStr, taskNode->arrivalTime);
    logToFile(logFile, "task%d: %d\n", taskId, cpuBurst);
    logToFile(logFile, "Arrival time: %s\n\n", timeStr);
    pthread_mutex_unlock(&logMutex);

    free(timeStr);
    free(str);
    free(node);

    timeStr = NULL;
    str = NULL;
    node = NULL;
}

void printCpuStat(File* logFile, int id, Task* task)
{
    char* timeStr;

    strTime(&timeStr, task->arrivalTime);
    logToFile(logFile, CPU_HEAD, id);
    logToFile(logFile, "Task %d\n", task->id);
    logToFile(logFile, "Arrival Time: %s\n", timeStr);

    free(timeStr);
    timeStr = NULL;
}

void strTime(char** str, time_t secs)
{
    struct tm* time = localtime(&secs);

    *str = (char*)malloc(sizeof(char) * 9);
    memset(*str, '\0', 9);
    sprintf(*str, TIME, time->tm_hour,
                        time->tm_min,
                        time->tm_sec);
}

void logToFile(File* file, char* format, ...)
{
    char* str;

    va_list(args);
    va_start(args, format);

    str = (char*)malloc(sizeof(char) * BUFSIZ);
    memset(str, '\0', BUFSIZ);

    vsprintf(str, format, args);

#ifdef DEBUG
    fprintf(stderr, "%s", str);
#endif

    addLineToFile(str, file);
}

void usage(char* exe)
{
    fprintf(stderr, "Usage: %s [task-file] [2-10]\n", exe);
}
