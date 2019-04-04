#include <stdlib.h>
#include <stdio.h>

#include "../file.h"
#include "../queue.h"

int main(int argc, char** argv)
{
    File f;
    QueueNode* n;

    char* str;
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
            readFile(argv[i], &f);

            while (! isQueueEmpty(f.data))
            {
                n = dequeue(f.data, (void*)&str, &j);
                fprintf(stdout, "%s\n", str);

                free(n->value);
                n->value = NULL;

                free(n);
                n = NULL;
            }

            fprintf(stdout, "\nFile rows: %d\n", f.rows);
            fprintf(stdout, "File cols: %d\n", f.cols);

            clearQueue(&(f.data));
        }
    }

    return 0;
}
