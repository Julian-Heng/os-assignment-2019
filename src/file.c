/**
 * Filename: file.c
 * Author:   Julian Heng (19473701)
 * Purpose:  Contains functions that deals with file input/output
 **/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "queue.h"

#define FALSE 0
#define TRUE !FALSE

/**
 * Reads a file to a queue
 * Takes a filename parameter and a File struct
 **/
void readFile(char* filename, File* file)
{
    FILE* f;
    int rows, cols, count, ch, i;
    char* str;

    f = fopen(filename, "r");

    if (! ferror(f))
    {
        rows = 0;
        cols = 0;
        count = 0;
        str = NULL;

        /* Get row and column count of the file */
        while ((ch = fgetc(f)) != EOF && ! ferror(f))
        {
            if (ch == '\n')
            {
                rows++;
                cols = count > cols ? count : cols;
                count = 0;
            }
            else
            {
                count++;
            }
        }

        fseek(f, 0, SEEK_SET);
        file->data = initQueue(rows);
        str = (char*)malloc((cols + 2) * sizeof(char));
        i = -1;

        /* Save line from file to str, then trim newline and enqueue */
        while ((++i < rows) && fgets(str, cols + 2, f))
        {
            str[strcspn(str, "\n")] = '\0';
            enqueue(file->data, str, TRUE);
            str = (char*)malloc((cols + 2) * sizeof(char));
        }

        /* Cleanup extra malloc call from loop */
        free(str);
        str = NULL;

        file->rows = rows;
        file->cols = cols;

        fclose(f);
    }
}
