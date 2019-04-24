#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "../file.h"
#include "../queue.h"

int main(int argc, char** argv)
{
    File* f;
    QueueNode* n;

    char* str;
    int i, j;

    if (argc == 1)
    {
        fprintf(stdout, "Usage: ./test_file_read filename\n");
    }
    else
    {
        for (i = 1; i < argc; i++)
        {
            fprintf(stdout, "\n");

            f = initFile(INT_MAX);
            setFilename(argv[i], f);
            readFile(f);

            while (! isQueueEmpty(f->data))
            {
                n = dequeue(f->data, (void*)&str, &j);
                fprintf(stdout, "%s\n", str);

                free(n->value);
                n->value = NULL;

                free(n);
                n = NULL;
            }

            fprintf(stdout, "\nFile name:%s\n", f->filename);
            fprintf(stdout, "File rows: %d\n", f->rows);
            fprintf(stdout, "File cols: %d\n", f->cols);

            freeFile(&f);
        }
    }

    return 0;
}
