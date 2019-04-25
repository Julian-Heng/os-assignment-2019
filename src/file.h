#ifndef FILE_H
#define FILE_H

#include "queue.h"

/**
 * A File struct to contain the filename, the data, rows and column count
 **/
typedef struct File
{
    char* filename;
    Queue* data;
    int rows;
    int cols;
} File;

File* initFile(int size);
void readFile(File* file);
void writeFile(File* file, char* mode);
void setFilename(char* filename, File* file);
void addLineToFile(char* line, File* file);
void updateFileCount(File* file);
void freeFile(File** file);

#endif
