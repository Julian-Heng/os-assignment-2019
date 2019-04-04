#include <stdlib.h>
#include <stdio.h>

#include "../file.h"

int main(int argc, char** argv)
{
    File f;
    int i, j;

    if (argc == 1)
    {
        fprintf(stdout, "Usage: ./test_file filename\n");
    }
    else
    {
        for (i = 1; i < argc; i++)
        {
            fprintf(stdout, "\n");
            read_file(argv[i], &f);
            for (j = 0; j < f.rows; j++)
            {
                fprintf(stdout, "%s\n", (f.d)[j]);
            }

            fprintf(stdout, "\nFile rows: %d\n", f.rows);
            fprintf(stdout, "File cols: %d\n", f.cols);

            for (j = 0; j < f.rows; j++)
            {
                free((f.d)[j]);
                (f.d)[j] = NULL;
            }

            free(f.d);
            f.d = NULL;
        }
    }

    return 0;
}
