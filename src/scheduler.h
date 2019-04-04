#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"

int run(char* filename, int max);
void process(int task, int proc);
void usage(void);

#endif
