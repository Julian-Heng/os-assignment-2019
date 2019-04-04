#ifndef FILE_H
#define FILE_H

typedef struct File
{
    char** arr;
    int rows;
    int cols;
} File;

void read_file(char* filename, File* file);

#endif
