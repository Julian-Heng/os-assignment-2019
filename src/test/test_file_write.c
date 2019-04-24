#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../file.h"
#include "../queue.h"

int main(int argc, char** argv)
{
    File* f;

    char* str;
    int i;

    if (argc == 1)
    {
        fprintf(stdout, "Usage: ./test_file_write filename\n");
    }
    else
    {
        f = initFile(INT_MAX);
        setFilename(argv[1], f);

        for (i = 0; i < 256; i++)
        {
            str = (char*)malloc(sizeof(char) * (i + 2));
            memset(str, '*', i);
            str[i] = '\n';
            str[i + 1] = '\0';
            addLineToFile(str, f);
        }

        writeFile(f, "w");
        freeFile(&f);
    }

    return 0;
}
