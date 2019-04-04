#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "file.h"

void read_file(char* filename, File* file)
{
    FILE* f;
    int rows, cols, count, ch, i;

    f = fopen(filename, "r");

    if (! ferror(f))
    {
        rows = 0;
        cols = 0;
        count = 0;

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
        file -> arr = (char**)malloc(rows * sizeof(char*));

        for (i = 0; i < rows; i++)
        {
            (file -> arr)[i] = (char*)malloc(cols + 1 * sizeof(char));
            memset((file -> arr)[i], '\0', cols + 1);
        }

        i = -1;
        while ((++i < rows) && fgets((file -> arr)[i], cols + 1, f))
        {
            (file -> arr)[i][strcspn((file -> arr)[i], "\n")] = '\0';
        }

        file -> rows = rows;
        file -> cols = cols;

        fclose(f);
    }
}
