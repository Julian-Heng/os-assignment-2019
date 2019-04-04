#ifndef FILE_H
#define FILE_H

#include "queue.h"

typedef struct File
{
    Queue* data;
    int rows;
    int cols;
} File;

void readFile(char* filename, File* file);

#endif
