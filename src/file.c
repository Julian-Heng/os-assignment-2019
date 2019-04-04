#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "file.h"

void readFile(char* filename, File* file)
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
        file -> d = (char**)malloc(rows * sizeof(char*));

        for (i = 0; i < rows; i++)
        {
            (file -> d)[i] = (char*)malloc((cols + 2) * sizeof(char));
            memset((file -> d)[i], '\0', cols + 2);
        }

        i = -1;
        while ((++i < rows) && fgets((file -> d)[i], cols + 2, f))
        {
            (file -> d)[i][strcspn((file -> d)[i], "\n")] = '\0';
        }

        file -> rows = rows;
        file -> cols = cols;

        fclose(f);
    }
}
