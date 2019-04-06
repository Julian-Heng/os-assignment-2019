#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "scheduler.h"
#include "file.h"
#include "queue.h"

#define FALSE 0
#define TRUE !FALSE

int main(int argc, char** argv)
{
    int ret;
    int max;

    if (argc < 3)
    {
        usage();
        ret = 1;
    }
    else
    {
        max = atoi(argv[2]);
        ret = run(argv[1], max);
    }

    return ret;
}

int run(char* filename, int max)
{
    File taskList;
    QueueNode* taskNode;
    Queue* readyQueue;
    Task* currentTask;

    int ret;
    int mallocCheck;

    readFile(filename, &taskList);
    readyQueue = initQueue(max);

    task(readyQueue, &taskList);
    task(readyQueue, &taskList);

    while (! isQueueEmpty(readyQueue))
    {
        taskNode = dequeue(readyQueue, (void**)&currentTask, &mallocCheck);
        process(currentTask);
        /*
        fprintf(stderr, "Task%d %d\n", currentTask->id, currentTask->time);
        */
        task(readyQueue, &taskList);
        task(readyQueue, &taskList);

        free(taskNode->value);
        free(taskNode);
    }

    clearQueue(&(taskList.data));
    clearQueue(&readyQueue);
    ret = 0;

    return ret;
}

void process(Task* task)
{
    time_t rawSecs;
    struct tm* timeinfo;

    char* arriveFmt;
    char* completeFmt;

    arriveFmt = "Arrival time: %02d:%02d:%02d\n";
    completeFmt = "Completion time: %02d:%02d:%02d\n\n";

    fprintf(stdout, "Task %d\n", task->id);

    time(&rawSecs);
    timeinfo = localtime(&rawSecs);
    fprintf(stdout, arriveFmt, timeinfo->tm_hour,
                               timeinfo->tm_min,
                               timeinfo->tm_sec);
    sleep(task->time);

    time(&rawSecs);
    timeinfo = localtime(&rawSecs);
    fprintf(stdout, completeFmt, timeinfo->tm_hour,
                                 timeinfo->tm_min,
                                 timeinfo->tm_sec);
}

void task(Queue* taskQueue, File* taskFile)
{
    Task* taskNode;
    QueueNode* node;

    char* str;
    int taskID;
    int time;
    int mallocCheck;

    if (! isQueueEmpty(taskFile->data) && ! isQueueFull(taskQueue))
    {
        taskNode = (Task*)malloc(sizeof(Task));
        node = dequeue(taskFile->data, (void**)&str, &mallocCheck);

        sscanf(str, "task%d %d", &taskID, &time);
        taskNode->id = taskID;
        taskNode->time = time;
        enqueue(taskQueue, taskNode, mallocCheck);

        free(str);
        str = NULL;

        free(node);
        node = NULL;
    }
}

void usage(void)
{
    fprintf(stdout, "Usage: scheduler [task-file]\n");
}
