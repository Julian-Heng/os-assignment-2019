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
    Queue* taskList;
    QueueNode* task;

    int taskID;
    int proc;
    int mallocCheck;

    char* str;

    if (argc == 1)
    {
        usage();
        return 1;
    }

    taskList = readTasks(argv[1]);

    while (! isQueueEmpty(taskList))
    {
        task = dequeue(taskList, (void**)&str, &mallocCheck);

        sscanf(str, "task%d %d", &taskID, &proc);
        process(taskID, proc);

        free(task -> value);
        free(task);
    }

    clearQueue(&taskList);

    return 0;
}

Queue* readTasks(char* filename)
{
    Queue* q;
    File f;
    int i;

    readFile(filename, &f);
    q = initQueue(f.rows);

    for (i = 0; i < f.rows; i++)
    {
        enqueue(q, f.d[i], TRUE);
    }

    free(f.d);
    f.d = NULL;

    return q;
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
    fprintf(stdout, arriveFmt, timeinfo -> tm_hour,
                               timeinfo -> tm_min,
                               timeinfo -> tm_sec);
    sleep(proc);

    time(&rawSecs);
    timeinfo = localtime(&rawSecs);
    fprintf(stdout, completeFmt, timeinfo -> tm_hour,
                                 timeinfo -> tm_min,
                                 timeinfo -> tm_sec);
}

void usage(void)
{
    fprintf(stdout, "Usage: scheduler [task-file]\n");
}
