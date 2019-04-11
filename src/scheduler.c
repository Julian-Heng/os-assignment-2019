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

#define ARRIVE_FMT   "Arrival time: %02d:%02d:%02d\n"
#define COMPLETE_FMT "Completion time: %02d:%02d:%02d\n\n"

pthread_mutex_t queueMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t taskMutex   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cpuMutex    = PTHREAD_MUTEX_INITIALIZER;

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
    pthread_t cpuThread;
    SharedData sharedData;

    Queue* readyQueue;
    File* taskFile;

    int ret;

    taskFile = (File*)malloc(sizeof(File));
    readyQueue = initQueue(max);
    readFile(filename, taskFile);

    sharedData.readyQueue = readyQueue;
    sharedData.taskFile = taskFile;

    pthread_create(&taskThread, NULL, task, &sharedData);
    pthread_create(&cpuThread, NULL, process, &sharedData);

    pthread_join(taskThread, NULL);
    pthread_join(cpuThread, NULL);

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

    printf("task(): sharedData = %p\n", (void*)sharedData);
    printf("task(): readyQueue = %p\n", (void*)readyQueue);
    printf("task(): taskFile = %p\n", (void*)taskFile);

    while (! isQueueEmpty(taskFile->data) ||
           ! isQueueEmpty(readyQueue))
    {
        printf("task(): Waiting for queueMutex\n");
        pthread_mutex_lock(&queueMutex);
        printf("task(): Got queueMutex\n");

        if (getQueueRemainingCapacity(readyQueue) < 2)
        {
            /* readyQueue is full */
            printf("task(): readyQueue is full\n");

            pthread_cond_signal(&queueFull);
            printf("task(): Sent queueFull signal\n");

            while (getQueueRemainingCapacity(readyQueue) < 2)
            {
                printf("task(): Waiting for queueEmpty signal\n");
                pthread_cond_wait(&queueEmpty, &queueMutex);
                printf("task(): Received queueEmpty signal\n");
            }
        }

        printf("task(): Adding tasks to readyQueue until full\n");
        while (getQueueRemainingCapacity(readyQueue) > 1 &&
               ! isQueueEmpty(taskFile->data))
        {
            if (! isQueueEmpty(taskFile->data))
            {
                taskThreadAddTask(readyQueue, taskFile);
            }

            if (! isQueueEmpty(taskFile->data))
            {
                taskThreadAddTask(readyQueue, taskFile);
            }

            printf("task(): Remaining tasks: %d\n",
                    getQueueLength(taskFile->data));
            printf("task(): Remaining readyQueue capacity: %d\n",
                    getQueueRemainingCapacity(readyQueue));
        }

        pthread_mutex_unlock(&queueMutex);
        printf("task(): Released queue lock\n");

        pthread_cond_signal(&queueFull);
        printf("task(): Sent queueFull signal\n");
    }

    printf("task(): Exiting thread\n");
    pthread_exit(NULL);
}

void* process(void* args)
{
    SharedData* sharedData = (SharedData*)args;
    Queue* readyQueue = sharedData->readyQueue;
    File* taskFile = sharedData->taskFile;

    QueueNode* node;
    Task* task;
    int isMalloc;

    time_t rawSecs;
    struct tm* timeinfo;

    printf("process(): sharedData = %p\n", (void*)sharedData);
    printf("process(): readyQueue = %p\n", (void*)readyQueue);
    printf("process(): taskFile = %p\n", (void*)taskFile);

    while (! isQueueEmpty(taskFile->data) ||
           ! isQueueEmpty(readyQueue))
    {
        printf("process(): Waiting for queueMutex\n");
        pthread_mutex_lock(&queueMutex);
        printf("process(): Got queueMutex\n");

        if (isQueueEmpty(readyQueue))
        {
            /* readyQueue is empty */
            printf("process(): readyQueue is empty\n");

            pthread_cond_signal(&queueEmpty);
            printf("process(): Sent queueEmpty signal\n");

            while (isQueueEmpty(readyQueue))
            {
                printf("process(): Waiting for queueFull signal\n");
                pthread_cond_wait(&queueFull, &queueMutex);
                printf("process(): Received queueFull signal\n");
            }
        }

        while (! isQueueEmpty(readyQueue))
        {
            node = dequeue(readyQueue, (void**)&task, &isMalloc);

            printf("Task %d\n", task->id);

            time(&rawSecs);
            timeinfo = localtime(&rawSecs);
            printf(ARRIVE_FMT, timeinfo->tm_hour,
                               timeinfo->tm_min,
                               timeinfo->tm_sec);
            sleep(task->time);
            time(&rawSecs);
            timeinfo = localtime(&rawSecs);
            printf(COMPLETE_FMT, timeinfo->tm_hour,
                                 timeinfo->tm_min,
                                 timeinfo->tm_sec);

            free(task);
            task = NULL;

            free(node);
            node = NULL;
        }

            pthread_mutex_unlock(&queueMutex);
            printf("process(): Released queue lock\n");

        /* readyQueue is empty */
        printf("process(): readyQueue is empty\n");

        pthread_mutex_unlock(&queueMutex);
        printf("task(): Released queue lock\n");

        pthread_cond_signal(&queueEmpty);
        printf("process(): Sent queueEmpty signal\n");
    }

    printf("process(): Exiting thread\n");
    pthread_exit(NULL);
}

void taskThreadAddTask(Queue* taskQueue, File* taskList)
{
    Task* taskNode;
    QueueNode* node;

    char* str;
    int taskId;
    int time;
    int isMalloc;

    taskNode = (Task*)malloc(sizeof(Task));
    node = dequeue(taskList->data, (void**)&str, &isMalloc);

    sscanf(str, "task%d %d", &taskId, &time);
    taskNode->id = taskId;
    taskNode->time = time;
    enqueue(taskQueue, taskNode, isMalloc);

    free(str);
    free(node);

    str = NULL;
    node = NULL;
}

void usage(char* exe)
{
    fprintf(stderr, "Usage: %s [task-file] [1-10]\n", exe);
}
