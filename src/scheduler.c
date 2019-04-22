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

#define TIME_FMT            "%02d:%02d:%02d"
#define TASK_TALLY_FMT      "Number of tasks put into Ready-Queue: %d\n"
#define CPU_TERMINATE_FMT   "CPU-%d terminates after servicing %d tasks\n"

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

        if (max <= 10 && max > 0)
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

    fprintf(stdout, "Number of tasks: %d\n", totalTasks);
    fprintf(stdout, RESULT_AVG_WAIT, INT_REAL_DIV(totalWaitingTime,
                                                  totalTasks));
    fprintf(stdout, RESULT_AVG_TURN, INT_REAL_DIV(totalTurnaroundTime,
                                                  totalTasks));

    clearQueue(&readyQueue);
    clearQueue(&(taskFile->data));

    free(taskFile);
    taskFile = NULL;

    ret = 0;
    return ret;
}

void* task(void* args)
{
    SharedData* sharedData = (SharedData*)args;
    Queue* readyQueue = sharedData->readyQueue;
    File* taskFile = sharedData->taskFile;

    int numTasks = 0;
    time_t rawSecs;

    while (! isQueueEmpty(taskFile->data))
    {
        pthread_mutex_lock(&queueMutex);

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
                taskThreadAddTask(readyQueue, taskFile);
                numTasks++;
            }

            if (! isQueueEmpty(taskFile->data))
            {
                taskThreadAddTask(readyQueue, taskFile);
                numTasks++;
            }
        }

        pthread_mutex_unlock(&queueMutex);

        pthread_cond_signal(&queueFull);
    }

    time(&rawSecs);

    pthread_mutex_lock(&logMutex);
    fprintf(stdout, TASK_TALLY_FMT, numTasks);
    fprintf(stdout, "Terminate at time: ");
    printTime(stdout, rawSecs);
    fprintf(stdout, "\n\n");
    pthread_mutex_unlock(&logMutex);

    pthread_exit(NULL);
}

void* process(void* args)
{
    CpuData* cpuData = (CpuData*)args;
    SharedData* sharedData = cpuData->data;
    int id = cpuData->id;

    Queue* readyQueue = sharedData->readyQueue;
    File* taskFile = sharedData->taskFile;

    QueueNode* node;
    Task* task;
    int isMalloc;

    while (! isQueueEmpty(taskFile->data) ||
           ! isQueueEmpty(readyQueue))
    {
        pthread_mutex_lock(&queueMutex);

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

        fprintf(stdout, "Statistics for CPU-%d\n", id);
        fprintf(stdout, "Task %d\n", task->id);
        fprintf(stdout, "Arrival Time: ");
        printTime(stdout, task->arrivalTime);
        fprintf(stdout, "\nService Time: ");
        printTime(stdout, task->serviceTime);
        fprintf(stdout, "\n\n");

        pthread_mutex_unlock(&logMutex);

        sleep(task->time);
        time(&(task->completionTime));
        (cpuData->totalTurnaroundTime) += (task->completionTime) -
                                          (task->arrivalTime);

        pthread_mutex_lock(&logMutex);

        fprintf(stdout, "Statistics for CPU-%d\n", id);
        fprintf(stdout, "Task %d\n", task->id);
        fprintf(stdout, "Arrival Time: ");
        printTime(stdout, task->arrivalTime);
        fprintf(stdout, "\nCompletion Time: ");
        printTime(stdout, task->completionTime);
        fprintf(stdout, "\n\n");

        pthread_mutex_unlock(&logMutex);

        free(task);
        task = NULL;

        free(node);
        node = NULL;
    }

    pthread_mutex_lock(&logMutex);
    fprintf(stdout, CPU_TERMINATE_FMT, id, cpuData->numTasks);
    pthread_mutex_unlock(&logMutex);

    pthread_exit(NULL);
}

void taskThreadAddTask(Queue* taskQueue, File* taskList)
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

    free(str);
    free(node);

    str = NULL;
    node = NULL;
}

void printTime(FILE* f, time_t secs)
{
    struct tm* timeinfo = localtime(&secs);
    fprintf(f, TIME_FMT, timeinfo->tm_hour,
                         timeinfo->tm_min,
                         timeinfo->tm_sec);
}

void usage(char* exe)
{
    fprintf(stderr, "Usage: %s [task-file] [1-10]\n", exe);
}
