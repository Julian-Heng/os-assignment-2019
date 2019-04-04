#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"

Queue* readTasks(char* filename);
void process(int task, int proc);
void usage(void);

#endif
