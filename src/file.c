/**
 * Filename: file.c
 * Author:   Julian Heng (19473701)
 * Purpose:  Contains functions that deals with file input/output
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "queue.h"

#define FALSE 0
#define TRUE !FALSE

File* initFile(int size)
{
    File* file;
    if ((file = (File*)malloc(sizeof(File))))
    {
        file->filename = NULL;
        file->data = initQueue(size);
        file->rows = 0;
        file->cols = 0;
    }

    return file;
}

/**
 * Reads a file to a queue
 * Takes a filename parameter and a File struct
 **/
void readFile(File* file)
{
    FILE* f;
    int cols, count, ch;
    char* str;

    f = fopen(file->filename, "r");

    if (! ferror(f) && file)
    {
        cols = 0;
        count = 0;
        str = NULL;

        /* Get column count of the file */
        while ((ch = fgetc(f)) != EOF && ! ferror(f))
        {
            if (ch == '\n')
            {
                cols = count > cols ? count : cols;
                count = 0;
            }
            else
            {
                count++;
            }
        }

        fseek(f, 0, SEEK_SET);

        str = (char*)malloc((cols + 2) * sizeof(char));

        /* Save line from file to str, then trim newline and enqueue */
        while (fgets(str, cols + 2, f))
        {
            str[strcspn(str, "\n")] = '\0';
            addLineToFile(str, file);
            str = (char*)malloc((cols + 2) * sizeof(char));
        }

        /* Cleanup extra malloc call from loop */
        free(str);
        str = NULL;

        fclose(f);
    }
}

void writeFile(File* file, char* mode)
{
    FILE* f;

    Queue* clone;
    QueueNode* node;
    char* str;
    int isMalloc;

    int length;

    f = fopen(file->filename, mode);
    str = NULL;

    if (! ferror(f) && file)
    {
        length = file->rows;
        clone = initQueue(length);

        while (! isQueueEmpty(file->data))
        {
            node = dequeue(file->data, (void*)&str, &isMalloc);
            fprintf(f, "%s", str);
            enqueue(clone, str, isMalloc);

            free(node);
            node = NULL;
        }

        clearQueue(&(file->data));
        file->data = clone;
        fclose(f);
    }
}

void setFilename(char* filename, File* file)
{
    int len;

    if (file)
    {
        len = strlen(filename) + 1;
        file->filename = malloc(sizeof(char) * len);
        strncpy(file->filename, filename, len);
    }
}

void addLineToFile(char* line, File* file)
{
    int len;

    len = strlen(line);
    file->cols = len > file->cols ? len : file->cols;
    enqueue(file->data, line, TRUE);
    (file->rows)++;
}

void freeFile(File** file)
{
    if ((*file)->filename)
    {
        free((*file)->filename);
        (*file)->filename = NULL;
    }

    clearQueue(&((*file)->data));

    free(*file);
    *file = NULL;
}
