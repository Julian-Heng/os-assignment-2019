#ifndef FILE_H
#define FILE_H

typedef struct File
{
    char** d;
    int rows;
    int cols;
} File;

void readFile(char* filename, File* file);

#endif
