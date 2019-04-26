/**
 * Filename: scheduler.c
 * Author:   Julian Heng (19473701)
 * Purpose:  The scheduler program that uses a first come first serve
 *           algorithm
 **/

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

#define TIME            "%02d:%02d:%02d"
#define TIME_LENGTH     9

/* String literals */
#define SEPARATOR       "-----\n"
#define FILE_ERROR      "Error: \"%s\" is not a valid task file\n"

#define TASK_SCAN       "task%d %d\n"
#define TASK_1          "task%d: %d\n"
#define TASK_2          "Task %d\n"
#define TASK_NUM        "Number of tasks: %d\n"
#define TASK_TALLY      "Number of tasks put into Ready-Queue: %d\n"
#define TASK_TERMINATE  "Terminate at time: %s\n"

#define CPU_HEAD        "Statistics for CPU-%d\n"
#define ARRIVAL_TIME    "Arrival Time: %s\n"
#define SERVICE_TIME    "Service Time: %s\n"
#define COMPLETE_TIME   "Completion Time: %s\n"
#define CPU_TERMINATE   "CPU-%d terminates after servicing %d tasks\n\n"

#define RESULT_AVG_WAIT "Average waiting time: %.3f secs\n"
#define RESULT_AVG_TURN "Average turnaround time: %.3f secs\n"

#define LOG_FILE        "simulation_log"

/**
 * queueMutex   Mutex lock for locking the readyQueue
 * logMutex     Mutex lock for locking the log file
 * statMutex    Mutex lock for updating the stats
 *
 * queueFull    Condition sent by task() to notify that the ready queue
 *              is full
 * queueEmpty   Condition sent by cpu() to notify that the ready queue
 *              is empty
 **/
pthread_mutex_t queueMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t logMutex    = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t statMutex   = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t queueFull    = PTHREAD_COND_INITIALIZER;
pthread_cond_t queueEmpty   = PTHREAD_COND_INITIALIZER;

/**
 * Main function
 **/
int main(int argc, char** argv)
{
    int ret;
    int max;

    /* Argument parsing and validation */
    if (argc < MIN_ARGS)
    {
        usage(argv[0]);
        ret = ERROR_ARGS;
    }
    else
    {
        max = atoi(argv[2]);

        if (max > 0 && 11 > max)
        {
            /* Arguments are valid, run program */
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

/**
 * The main algorithm in the scheduler program
 *
 * filename is the name for the task file
 * max is the maximum readyQueue size
 **/
int run(char* filename, int max)
{
    /**
     * Task will be assigned with a SharedData struct
     * Each cpu thread is assigned a CpuData struct, which all point to
     * the same SharedData struct in Task. Each CpuData has an int,
     * which refers to the CPU id
     **/
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

    /**
     * Linking the shared data to the variables in this function
     * stack
     **/
    sharedData.readyQueue = readyQueue;
    sharedData.taskFile = taskFile;
    sharedData.logFile = logFile;

    sharedData.numTasks = &numTasks;
    sharedData.totalWaitingTime = &totalWaitingTime;
    sharedData.totalTurnaroundTime = &totalTurnaroundTime;

    if (validateTaskFile(taskFile))
    {
        /* Start of the log file */
        logger(sharedData.logFile, SEPARATOR);

        /* Set up cpuData */
        for (i = 0; i < NUM_THREADS; i++)
        {
            cpuData[i].data = &sharedData;
            cpuData[i].id = i + 1;
        }

        /* Create threads */
        pthread_create(&taskThread, NULL, task, &sharedData);
        for (i = 0; i < NUM_THREADS; i++)
        {
            pthread_create(&cpuThread[i], NULL, cpu, &cpuData[i]);
        }

        /* Waiting for threads */
        pthread_join(taskThread, NULL);
        for (i = 0; i < NUM_THREADS; i++)
        {
            pthread_join(cpuThread[i], NULL);
        }

        /* Log result */
        logger(logFile, TASK_NUM, numTasks);
        logger(logFile, RESULT_AVG_WAIT, INT_REAL_DIV(totalWaitingTime,
                                                      numTasks));
        logger(logFile, RESULT_AVG_TURN, INT_REAL_DIV(totalTurnaroundTime,
                                                      numTasks));

        /* Write log file */
        writeFile(logFile, "a");
    }
    else
    {
        fprintf(stderr, FILE_ERROR, taskFile->filename);
    }

    /* Cleanup */
    clearQueue(&readyQueue);
    freeFile(&taskFile);
    freeFile(&logFile);

    return SUCCESS;
}

/**
 * The runnable task thread
 **/
void* task(void* args)
{
    /**
     * Typecasting args to the individual variables
     * All of these variables are shared
     **/
    SharedData* sharedData = (SharedData*)args;
    Queue* readyQueue = sharedData->readyQueue;
    File* taskFile = sharedData->taskFile;
    File* logFile = sharedData->logFile;

    /* These variables are local to task */
    char* timeStr;
    int numTasks;
    time_t rawSecs;

    numTasks = 0;

    while (TRUE)
    {
        /* Acquire lock */
        pthread_mutex_lock(&queueMutex);

        /* Check for exit condition */
        if (isQueueEmpty(taskFile->data))
        {
            /* Release lock*/
            pthread_mutex_unlock(&queueMutex);
            time(&rawSecs);

            /* Log task exiting */
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

        /* Different logic for a readyQueue with a max size of 1 */
        if (getQueueMaxLength(readyQueue) == 1)
        {
            if (isQueueFull(readyQueue))
            {
                pthread_cond_broadcast(&queueFull);

                while (isQueueFull(readyQueue))
                {
                    pthread_cond_wait(&queueEmpty, &queueMutex);
                }
            }

            if (isQueueEmpty(readyQueue) &&
                ! isQueueEmpty(taskFile->data))
            {
                /* Add task */
                taskThreadAddTask(readyQueue, taskFile, logFile);

                /* Wake cpu thread */
                pthread_cond_broadcast(&queueFull);
                numTasks++;
            }
        }
        else
        {
            /* Check if readyQueue has less than 2 free slots */
            if (getQueueRemainingCapacity(readyQueue) < 2)
            {
                /* readyQueue is full */
                pthread_cond_broadcast(&queueFull);

                /**
                 * Wait for the queue to be empty/not full The task
                 * threads is blocked until the readyQueue is not full
                 **/
                while (getQueueRemainingCapacity(readyQueue) < 2)
                {
                    pthread_cond_wait(&queueEmpty, &queueMutex);
                }
            }

            /* Add tasks into readyQueue if there is space to add 2
             * Cpu thread can start working on the tasks as soon as
             * they're added
             **/
            if (getQueueRemainingCapacity(readyQueue) > 1 &&
                ! isQueueEmpty(taskFile->data))
            {
                if (! isQueueEmpty(taskFile->data))
                {
                    /* Add task */
                    taskThreadAddTask(readyQueue, taskFile, logFile);

                    /* Wake cpu thread */
                    pthread_cond_broadcast(&queueFull);
                    numTasks++;
                }

                /* Run again to add twice */
                if (! isQueueEmpty(taskFile->data))
                {
                    taskThreadAddTask(readyQueue, taskFile, logFile);
                    pthread_cond_broadcast(&queueFull);
                    numTasks++;
                }
            }
        }

        /* Release queue lock */
        pthread_mutex_unlock(&queueMutex);
    }
}

/**
 * The runnable cpu thread
 **/
void* cpu(void* args)
{
    /**
     * Typecasting args to the individual variables
     * All of these variables are shared
     **/
    CpuData* cpuData = (CpuData*)args;
    SharedData* sharedData = cpuData->data;
    int id = cpuData->id;

    Queue* readyQueue = sharedData->readyQueue;
    File* taskFile = sharedData->taskFile;
    File* logFile = sharedData->logFile;

    /* These variables are local to each cpu thread */
    QueueNode* node;
    Task* task;
    int isMalloc;

    int numTasks;
    char* timeStr;

    numTasks = 0;

    while (TRUE)
    {
        /* Acquire queue lock */
        pthread_mutex_lock(&queueMutex);

        /* Check for exit condition */
        if (isQueueEmpty(taskFile->data) && isQueueEmpty(readyQueue))
        {
            /* Release lock*/
            pthread_mutex_unlock(&queueMutex);

            /* Log task exiting */
            pthread_mutex_lock(&logMutex);
            logger(logFile, CPU_TERMINATE, id, numTasks);
            pthread_mutex_unlock(&logMutex);

            pthread_exit(NULL);
        }

        if (isQueueEmpty(readyQueue))
        {
            /* readyQueue is empty */
            pthread_cond_signal(&queueEmpty);

            /**
             * Wait for the queue to be full/not empty
             * The cpu thread is blocked until the readyQueue is not
             * empty
             **/
            while (isQueueEmpty(readyQueue))
            {
                pthread_cond_wait(&queueFull, &queueMutex);
            }
        }

        /* Remove node from readyQueue */
        node = dequeue(readyQueue, (void**)&task, &isMalloc);

        /* Signal to task to start filling the readyQueue */
        pthread_cond_signal(&queueEmpty);

        /* Release queue lock */
        pthread_mutex_unlock(&queueMutex);

        /* Start "processing" task */
        time(&(task->serviceTime));

        /* Update scheduler stats */
        pthread_mutex_lock(&statMutex);
        numTasks++;
        (*(sharedData->numTasks))++;
        (*(sharedData->totalWaitingTime)) += (task->serviceTime) -
                                             (task->arrivalTime);
        pthread_mutex_unlock(&statMutex);

        /* Log service time */
        pthread_mutex_lock(&logMutex);
        strTime(&timeStr, task->serviceTime);
        logCpuStat(logFile, id, task);
        logger(logFile, SERVICE_TIME, timeStr);
        logger(logFile, "\n");
        free(timeStr);
        timeStr = NULL;
        pthread_mutex_unlock(&logMutex);

        /* "Process" the task */
        sleep(task->time);
        time(&(task->completionTime));

        pthread_mutex_lock(&statMutex);
        (*(sharedData->totalTurnaroundTime)) += (task->completionTime) -
                                                (task->arrivalTime);
        pthread_mutex_unlock(&statMutex);

        /* Log completion time */
        pthread_mutex_lock(&logMutex);
        strTime(&timeStr, task->completionTime);
        logCpuStat(logFile, id, task);
        logger(logFile, COMPLETE_TIME, timeStr);
        logger(logFile, "\n");
        pthread_mutex_unlock(&logMutex);

        /* Cleanup */
        free(timeStr);
        free(task);
        free(node);

        timeStr = NULL;
        task = NULL;
        node = NULL;

        /**
         * Restart back at the top, if it detects that the queue is
         * empty, send a queue empty signal and start again when there
         * is an item the ready queue
         **/
    }
}

/**
 * Function used by task thread to add a task from the task list to the
 * ready queue, logging the process
 *
 * This function is assumed to have the queueMutex lock
 **/
void taskThreadAddTask(Queue* readyQueue, File* taskFile, File* logFile)
{
    Task* taskNode;
    QueueNode* node;

    char* timeStr;
    char* str;
    int isMalloc;

    /* Allocate memory for a Task struct */
    taskNode = (Task*)malloc(sizeof(Task));

    /* Get a line from the task file */
    node = dequeue(taskFile->data, (void**)&str, &isMalloc);

    /* Parse string */
    sscanf(str, TASK_SCAN, &(taskNode->id), &(taskNode->time));
    taskNode->cpu = INT_MAX;
    time(&(taskNode->arrivalTime));

    /* Add to the readyQueue */
    enqueue(readyQueue, taskNode, isMalloc);

    /* Log arrival time */
    pthread_mutex_lock(&logMutex);
    strTime(&timeStr, taskNode->arrivalTime);
    logger(logFile, TASK_1, taskNode->id, taskNode->time);
    logger(logFile, ARRIVAL_TIME, timeStr);
    logger(logFile, "\n");
    pthread_mutex_unlock(&logMutex);

    /* Cleanup */
    free(timeStr);
    free(str);
    free(node);

    timeStr = NULL;
    str = NULL;
    node = NULL;
}

/**
 * Function to log common information to the log file
 * Logs task number and arrival time
 **/
void logCpuStat(File* logFile, int id, Task* task)
{
    char* timeStr;

    strTime(&timeStr, task->arrivalTime);
    logger(logFile, CPU_HEAD, id);
    logger(logFile, TASK_2, task->id);
    logger(logFile, ARRIVAL_TIME, timeStr);

    free(timeStr);
    timeStr = NULL;
}

/**
 * Fills a string with the current time
 **/
void strTime(char** str, time_t secs)
{
    struct tm* time = localtime(&secs);

    *str = (char*)malloc(sizeof(char) * TIME_LENGTH);
    memset(*str, '\0', TIME_LENGTH);
    sprintf(*str, TIME, time->tm_hour,
                        time->tm_min,
                        time->tm_sec);
}

/**
 * Adds a line to the log file
 * Takes in the same arguments in the style of a printf function
 **/
void logger(File* file, char* format, ...)
{
    char* str;

    va_list(args);
    va_start(args, format);

    str = (char*)malloc(sizeof(char) * BUFSIZ);
    memset(str, '\0', BUFSIZ);

    vsprintf(str, format, args);

    /* Print to stderr if DEBUG flag is set */
#ifdef DEBUG
    fprintf(stderr, "%s", str);
#endif
    addLineToFile(str, file);
}

/**
 * Function to validate the task file
 * If the task file contains a line that doesn't have the right format,
 * it will return false
 **/
int validateTaskFile(File* file)
{
    int result = TRUE;
    Queue* clone;
    QueueNode* node;
    char* str;
    int isMalloc;

    int dummyId;
    int dummyTime;

    if (file)
    {
        str = NULL;
        clone = initQueue(file->rows);

        /**
         * Just like writeFile in file.c, we need to create a clone of
         * the file
         **/
        while (! isQueueEmpty(file->data) && result)
        {
            node = dequeue(file->data, (void*)&str, &isMalloc);
            enqueue(clone, str, isMalloc);
            result = sscanf(str, TASK_SCAN, &dummyId, &dummyTime) == 2;

            free(node);
            node = NULL;
        }

        clearQueue(&(file->data));
        file->data = clone;
    }

    return result;
}

/**
 * Print usage message
 **/
void usage(char* exe)
{
    fprintf(stderr, "Usage: %s [task-file] [2-10]\n", exe);
}
