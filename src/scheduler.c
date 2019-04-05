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
    QueueNode* task;

    int ret;
    int taskID;
    int proc;
    int mallocCheck;
    char* str;

    readFile(filename, &taskList);

    while (! isQueueEmpty(taskList.data))
    {
        task = dequeue(taskList.data, (void**)&str, &mallocCheck);

        sscanf(str, "task%d %d", &taskID, &proc);
        process(taskID, proc);

        free(task->value);
        free(task);
    }

    clearQueue(&(taskList.data));
    ret = 0;

    return ret;
}

void process(int task, int proc)
{
    time_t rawSecs;
    struct tm* timeinfo;

    char* arriveFmt;
    char* completeFmt;

    arriveFmt = "Arrival time: %02d:%02d:%02d\n";
    completeFmt = "Completion time: %02d:%02d:%02d\n\n";

    fprintf(stdout, "Task %d\n", task);

    time(&rawSecs);
    timeinfo = localtime(&rawSecs);
    fprintf(stdout, arriveFmt, timeinfo->tm_hour,
                               timeinfo->tm_min,
                               timeinfo->tm_sec);
    sleep(proc);

    time(&rawSecs);
    timeinfo = localtime(&rawSecs);
    fprintf(stdout, completeFmt, timeinfo->tm_hour,
                                 timeinfo->tm_min,
                                 timeinfo->tm_sec);
}

void usage(void)
{
    fprintf(stdout, "Usage: scheduler [task-file]\n");
}
