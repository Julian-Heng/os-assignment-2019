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

/**
 * Allocates memory for a file struct
 * Initialise all variables to default
 * Takes in a string for filename and an int for the maximum row
 * size of the file
 **/
File* initFile(char* filename, int size)
{
    File* file;
    if ((file = (File*)malloc(sizeof(File))))
    {
        file->filename = filename;
        file->data = initQueue(size);
        file->rows = 0;
        file->cols = 0;
    }

    return file;
}

/**
 * Reads a file to a queue
 * Takes in a File struct
 **/
void readFile(File* file)
{
    FILE* f;
    char* str;

    f = fopen(file->filename, "r");

    if (f && file && ! ferror(f))
    {
        str = NULL;

        /**
         * fgets scans a line from the file, including the newline,
         * so we would have to replace the newline with a null
         * terminator to trim the string before enqueueing to the
         * file queue
         **/
        while ((str = (char*)malloc(sizeof(char) * BUFSIZ)) &&
               fgets(str, BUFSIZ, f) &&
               ! ferror(f))
        {
            str[strcspn(str, "\n")] = '\0';
            addLineToFile(file, str);
        }

        /* Cleanup extra malloc call from loop */
        free(str);
        str = NULL;

        fclose(f);
    }
}

/**
 * Flush the contents of the file queue to a file
 * Takes in a string for the mode for fopen and a File struct
 **/
void writeFile(File* file, char* mode)
{
    FILE* f;

    QueueNode* node;
    char* str;
    int isMalloc;
    int length;

    f = fopen(file->filename, mode);
    str = NULL;

    if (f && file && ! ferror(f))
    {
        length = file->rows;

        /* Loop through the file queue */
        while (length-- > 0)
        {
            /* Dequeue and enqueue the file */
            node = dequeue(file->data, (void*)&str, &isMalloc);
            fprintf(f, "%s", str);
            enqueue(file->data, str, isMalloc);

            free(node);
            node = NULL;
        }

        fclose(f);
    }
}

/**
 * Enqueue a line to a file
 * The line needs to be constructed in the calling function
 **/
void addLineToFile(File* file, char* line)
{
    int len = strlen(line);

    /**
     * Check if the input line is longer than the current
     * longest line in the file
     **/
    file->cols = MAX(len, file->cols);
    enqueue(file->data, line, TRUE);
    (file->rows)++;
}

/**
 * Cleanup a File struct
 * Frees the queue and the filename variable
 **/
void freeFile(File** file)
{
    if (file)
    {
        (*file)->filename = NULL;

        clearQueue(&((*file)->data));

        free(*file);
        *file = NULL;
    }
}
