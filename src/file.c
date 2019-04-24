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
 * Takes in an int for the maximum row size of the file
 **/
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
 * Takes in a File struct
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

        /* Set file pointer to the start of the file */
        fseek(f, 0, SEEK_SET);

        /* Plus two for newline and string terminator */
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

/**
 * Flush the contents of the file queue to a file
 * Takes in a string for the mode for fopen and a File struct
 **/
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

    /* Check for file errors and if the File struct is not null */
    if (! ferror(f) && file)
    {
        /**
         * Create a duplicate File queue because dequeue removes them
         * from the original File queue
         **/
        length = file->rows;
        clone = initQueue(length);

        /* Loop until File queue is empty */
        while (! isQueueEmpty(file->data))
        {
            /* Dequeue and enqueue the file contents */
            node = dequeue(file->data, (void*)&str, &isMalloc);
            fprintf(f, "%s", str);
            enqueue(clone, str, isMalloc);

            free(node);
            node = NULL;
        }

        /* Clean the original queue and reassign to the File struct */
        clearQueue(&(file->data));
        file->data = clone;
        fclose(f);
    }
}

/**
 * Set the filename in a File struct
 * Takes in the filename and a File struct
 **/
void setFilename(char* filename, File* file)
{
    int len;

    /* If file is not null */
    if (file)
    {
        /* All filename in a File struct is the heap */
        len = strlen(filename) + 1;
        file->filename = malloc(sizeof(char) * len);
        strncpy(file->filename, filename, len);
    }
}

/**
 * Enqueue a line to a file
 * The line needs to be constructed in the calling function
 **/
void addLineToFile(char* line, File* file)
{
    int len = strlen(line);

    /**
     * Check if the input line is longer than the current
     * longest line in the file
     **/
    file->cols = len > file->cols ? len : file->cols;
    enqueue(file->data, line, TRUE);
    (file->rows)++;
}

/**
 * Cleanup a File struct
 * Frees the queue and the filename variable
 **/
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
