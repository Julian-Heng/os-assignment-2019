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

#define MIN_ARGS 3
#define NUM_THREADS 3

#define SUCCESS 0
#define ERROR_ARGS 1
#define ERROR_OUT_OF_BOUNDS 2

#define TIME                "%02d:%02d:%02d"
#define TIME_LENGTH         9

#define SEPARATOR           "-----\n"

#define TASK_SCAN           "task%d %d\n"
#define TASK_1              "task%d: %d\n"
#define TASK_2              "Task %d\n"
#define TASK_NUM            "Number of tasks: %d\n"
#define TASK_TALLY          "Number of tasks put into Ready-Queue: %d\n"
#define TASK_TERMINATE      "Terminate at time: %s\n"

#define CPU_HEAD            "Statistics for CPU-%d\n"
#define ARRIVAL_TIME        "Arrival Time: %s\n"
#define SERVICE_TIME        "Service Time: %s\n"
#define COMPLETE_TIME       "Completion Time: %s\n"
#define CPU_TERMINATE       "CPU-%d terminates after servicing %d tasks\n\n"

#define RESULT_AVG_WAIT     "Average waiting time: %.3f secs\n"
#define RESULT_AVG_TURN     "Average turnaround time: %.3f secs\n"

#define LOG_FILE            "simulation_log"

pthread_mutex_t queueMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t logMutex    = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t statMutex   = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t queueFull    = PTHREAD_COND_INITIALIZER;
pthread_cond_t queueEmpty   = PTHREAD_COND_INITIALIZER;

int main(int argc, char** argv)
{
    int ret;
    int max;

    if (argc < MIN_ARGS)
    {
        usage(argv[0]);
        ret = ERROR_ARGS;
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
            ret = ERROR_OUT_OF_BOUNDS;
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

    int numTasks;
    int totalWaitingTime;
    int totalTurnaroundTime;

    int i;

    numTasks = 0;
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

    sharedData.numTasks = &numTasks;
    sharedData.totalWaitingTime = &totalWaitingTime;
    sharedData.totalTurnaroundTime = &totalTurnaroundTime;

    logger(sharedData.logFile, SEPARATOR);

    for (i = 0; i < NUM_THREADS; i++)
    {
        cpuData[i].data = &sharedData;
        cpuData[i].id = i + 1;
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

    logger(logFile, TASK_NUM, numTasks);
    logger(logFile, RESULT_AVG_WAIT, INT_REAL_DIV(totalWaitingTime,
                                                  numTasks));
    logger(logFile, RESULT_AVG_TURN, INT_REAL_DIV(totalTurnaroundTime,
                                                  numTasks));

    clearQueue(&readyQueue);

    writeFile(logFile, "a");
    freeFile(&logFile);
    freeFile(&taskFile);

    return SUCCESS;
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
            logger(logFile, TASK_TALLY, numTasks);
            logger(logFile, TASK_TERMINATE, timeStr);
            logger(logFile, "\n");
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

    int numTasks;
    char* timeStr;

    numTasks = 0;

    while (TRUE)
    {
        pthread_mutex_lock(&queueMutex);

        if (isQueueEmpty(taskFile->data) && isQueueEmpty(readyQueue))
        {
            pthread_mutex_unlock(&queueMutex);
            pthread_mutex_lock(&logMutex);
            logger(logFile, CPU_TERMINATE, id, numTasks);
            pthread_mutex_unlock(&logMutex);

            pthread_exit(NULL);
        }

        if (isQueueEmpty(readyQueue))
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
        pthread_mutex_lock(&statMutex);
        numTasks++;
        (*(sharedData->numTasks))++;
        (*(sharedData->totalWaitingTime)) += (task->serviceTime) -
                                             (task->arrivalTime);
        pthread_mutex_unlock(&statMutex);


        pthread_mutex_lock(&logMutex);
        strTime(&timeStr, task->serviceTime);
        printCpuStat(logFile, id, task);
        logger(logFile, SERVICE_TIME, timeStr);
        logger(logFile, "\n");
        free(timeStr);
        timeStr = NULL;
        pthread_mutex_unlock(&logMutex);


        sleep(task->time);
        time(&(task->completionTime));


        pthread_mutex_lock(&statMutex);
        (*(sharedData->totalTurnaroundTime)) += (task->completionTime) -
                                                (task->arrivalTime);
        pthread_mutex_unlock(&statMutex);


        pthread_mutex_lock(&logMutex);
        strTime(&timeStr, task->completionTime);
        printCpuStat(logFile, id, task);
        logger(logFile, COMPLETE_TIME, timeStr);
        logger(logFile, "\n");
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
    int isMalloc;

    taskNode = (Task*)malloc(sizeof(Task));
    node = dequeue(taskList->data, (void**)&str, &isMalloc);

    sscanf(str, TASK_SCAN, &(taskNode->id), &(taskNode->time));
    taskNode->cpu = INT_MAX;
    time(&(taskNode->arrivalTime));
    enqueue(taskQueue, taskNode, isMalloc);

    pthread_mutex_lock(&logMutex);
    strTime(&timeStr, taskNode->arrivalTime);
    logger(logFile, TASK_1, taskNode->id, taskNode->time);
    logger(logFile, ARRIVAL_TIME, timeStr);
    logger(logFile, "\n");
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
    logger(logFile, CPU_HEAD, id);
    logger(logFile, TASK_2, task->id);
    logger(logFile, ARRIVAL_TIME, timeStr);

    free(timeStr);
    timeStr = NULL;
}

void strTime(char** str, time_t secs)
{
    struct tm* time = localtime(&secs);

    *str = (char*)malloc(sizeof(char) * TIME_LENGTH);
    memset(*str, '\0', TIME_LENGTH);
    sprintf(*str, TIME, time->tm_hour,
                        time->tm_min,
                        time->tm_sec);
}

void logger(File* file, char* format, ...)
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
