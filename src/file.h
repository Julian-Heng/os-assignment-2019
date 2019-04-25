#ifndef FILE_H
#define FILE_H

#include "queue.h"

/**
 * A File struct to contain the filename, the data, rows and column count
 **/
typedef struct File
{
    /**
     * filename - A string representing the file's filename
     * data     - A queue for holding each line in the file
     * rows     - An int for the length of the file
     * cols     - An int for the maximum column size in the file
     **/

    char* filename;
    Queue* data;
    int rows;
    int cols;
} File;

/* Function prototypes */
File* initFile(int size);
void readFile(File* file);
void writeFile(File* file, char* mode);
void setFilename(char* filename, File* file);
void addLineToFile(char* line, File* file);
void updateFileCount(File* file);
void freeFile(File** file);

#endif /* End of file.h */
