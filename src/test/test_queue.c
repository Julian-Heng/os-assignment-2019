#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../linkedList.h"
#include "../queue.h"
#include "test_queue.h"

#define FALSE 0
#define TRUE !FALSE

int main(void)
{
    int count = 0;
    int status = TRUE;
    int (*funcs[5])(void) = {
        testQueueConstructor,
        testQueueEnqueue,
        testQueueDequeue,
        testQueuePeek,
        testQueueClear
    };

    while (status && count < 5)
    {
        status = (*funcs[count++])();
    }

    return status == TRUE ? FALSE : TRUE;
}

int testQueueConstructor()
{
    Queue* q = NULL;
    int status = TRUE;

    header("Queue Constructor");

    if (status)
    {
        fprintf(stdout, "Testing initQueue(): ");
        q = initQueue();
        status = printResult(!! q && isQueueEmpty(q));
        free(q);
        q = NULL;
    }

    return status;
}

int testQueueEnqueue()
{
    Queue* q = NULL;
    QueueNode* n = NULL;
    char* str = NULL;
    int i;
    int status = TRUE;

    header("Queue Enqueue");

    if (status)
    {
        fprintf(stdout, "Testing enqueue() without a value: ");
        q = initQueue();
        enqueue(q, NULL, FALSE);
        status = printResult(!! q &&
                             ! (q -> head -> isMalloc) &&
                             ! (q -> head -> value));

        free(q -> head);
        q -> head = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing enqueue() with a value and not malloc: ");
        q = initQueue();
        str = "test string";
        enqueue(q, str, FALSE);
        status = printResult(!! q &&
                             ! (q -> head -> isMalloc) &&
                             ! strcmp(q -> head -> value, "test string"));

        free(q -> head);
        q -> head = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing enqueue() with a value and malloc: ");
        q = initQueue();
        str = (char*)malloc(sizeof(char) * (strlen("test string") + 1));
        strncpy(str, "test string", strlen("test string") + 1);
        enqueue(q, str, TRUE);
        status = printResult(!! q &&
                             q -> head -> isMalloc &&
                             ! strcmp(q -> head -> value, "test string"));

        free(q -> head -> value);
        q -> head -> value = NULL;

        free(q -> head);
        q -> head = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with a value and not malloc [5x]: ");
        q = initQueue();

        for (i = 0; i < 5; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        i = 0;
        n = q -> head;

        while (n != NULL && i++ < 5 && status)
        {
            if (!! q && ! strcmp(n -> value, "test string"))
            {
                status = TRUE;
                n = n -> next;
            }
            else
            {
                status = FALSE;
            }
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;
            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with a value and is malloc [5x]: ");
        q = initQueue();

        for (i = 0; i < 5; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        i = -1;
        n = q -> head;

        while (n != NULL && i++ < 5 && status)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);

            if (!! q && ! strcmp(n -> value, str))
            {
                status = TRUE;
                n = n -> next;
            }
            else
            {
                status = FALSE;
            }

            free(str);
            str = NULL;
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;

            free(n -> value);
            n -> value = NULL;

            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with value and mix malloc [5x]: ");

        q = initQueue();

        for (i = 0; i < 5; i++)
        {
            if (i & 1)
            {
                str = "test string";
                enqueue(q, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
                memset(str, '\0', (strlen("test string") + 3));
                sprintf(str, "test string %d", i);
                enqueue(q, str, TRUE);
            }
        }

        i = -1;
        n = q -> head;

        while (n != NULL && i++ < 5 && status)
        {
            if (i & 1)
            {
                if (!! q && ! strcmp(n -> value, "test string"))
                {
                    status = TRUE;
                    n = n -> next;
                }
                else
                {
                    status = FALSE;
                }
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
                memset(str, '\0', (strlen("test string") + 3));
                sprintf(str, "test string %d", i);

                if (!! q && ! strcmp(n -> value, str))
                {
                    status = TRUE;
                    n = n -> next;
                }
                else
                {
                    status = FALSE;
                }

                free(str);
                str = NULL;
            }
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;

            if (n -> isMalloc)
            {
                free(n -> value);
                n -> value = NULL;
            }

            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with a value and not malloc [100000x]: ");
        q = initQueue();

        for (i = 0; i < 100000; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        i = 0;
        n = q -> head;

        while (n != NULL && i++ < 100000 && status)
        {
            if (!! q && ! strcmp(n -> value, "test string"))
            {
                status = TRUE;
                n = n -> next;
            }
            else
            {
                status = FALSE;
            }
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;
            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with a value and is malloc [100000x]: ");
        q = initQueue();

        for (i = 0; i < 100000; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        i = -1;
        n = q -> head;

        while (n != NULL && i++ < 100000 && status)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);

            if (!! q && ! strcmp(n -> value, str))
            {
                status = TRUE;
                n = n -> next;
            }
            else
            {
                status = FALSE;
            }

            free(str);
            str = NULL;
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;

            free(n -> value);
            n -> value = NULL;

            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with value and mix malloc [100000x]: ");

        q = initQueue();

        for (i = 0; i < 100000; i++)
        {
            if (i & 1)
            {
                str = "test string";
                enqueue(q, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
                memset(str, '\0', (strlen("test string") + 9));
                sprintf(str, "test string %d", i);
                enqueue(q, str, TRUE);
            }
        }

        i = -1;
        n = q -> head;

        while (n != NULL && i++ < 100000 && status)
        {
            if (i & 1)
            {
                if (!! q && ! strcmp(n -> value, "test string"))
                {
                    status = TRUE;
                    n = n -> next;
                }
                else
                {
                    status = FALSE;
                }
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
                memset(str, '\0', (strlen("test string") + 9));
                sprintf(str, "test string %d", i);

                if (!! q && ! strcmp(n -> value, str))
                {
                    status = TRUE;
                    n = n -> next;
                }
                else
                {
                    status = FALSE;
                }

                free(str);
                str = NULL;
            }
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;

            if (n -> isMalloc)
            {
                free(n -> value);
                n -> value = NULL;
            }

            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    return status;
}

int testQueueDequeue()
{
    Queue* q = NULL;
    QueueNode* n = NULL;
    void* ptr = NULL;
    char* str = NULL;
    int isMalloc;
    int i;
    int status = TRUE;

    header("Queue Dequeue");

    if (status)
    {
        fprintf(stdout, "Testing dequeue() with an empty queue: ");
        q = initQueue();
        n = dequeue(q, &ptr, &isMalloc);
        status = printResult(!! q &&
                             ! n &&
                             ! ptr &&
                             isMalloc == -1);

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing dequeue() without a value: ");
        q = initQueue();
        enqueue(q, NULL, FALSE);
        n = dequeue(q, &ptr, &isMalloc);
        status = printResult(!! q &&
                             n &&
                             ! ptr &&
                             ! isMalloc);

        free(n);
        n = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing dequeue() with a value and not malloc: ");
        q = initQueue();
        str = "test string";
        enqueue(q, str, FALSE);
        n = dequeue(q, &ptr, &isMalloc);
        status = printResult(!! q &&
                             n &&
                             ptr &&
                             ! isMalloc &&
                             ! strcmp((char*)ptr, "test string"));

        free(n);
        n = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing dequeue() with a value and malloc: ");
        q = initQueue();
        str = (char*)malloc(sizeof(char) * (strlen("test string") + 1));
        strncpy(str, "test string", strlen("test string") + 1);
        enqueue(q, str, TRUE);
        n = dequeue(q, &ptr, &isMalloc);
        status = printResult(!! q &&
                             n &&
                             ptr &&
                             isMalloc &&
                             ! strcmp((char*)ptr, "test string"));

        free(ptr);
        ptr = NULL;

        free(n);
        n = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with a value and not malloc [5x]: ");
        q = initQueue();

        for (i = 0; i < 5; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        i = 0;

        while ((n = dequeue(q, &ptr, &isMalloc)) && status)
        {
            if (n && ! strcmp((char*)ptr, "test string") && ! isMalloc)
            {
                status = TRUE;
            }
            else
            {
                status = FALSE;
            }

            free(n);
            n = NULL;
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;
            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with a value and is malloc [5x]: ");
        q = initQueue();

        for (i = 0; i < 5; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        i = -1;

        while ((n = dequeue(q, &ptr, &isMalloc)) && status)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", ++i);

            if (! strcmp((char*)ptr, str) && isMalloc)
            {
                status = TRUE;
            }
            else
            {
                status = FALSE;
            }

            free(ptr);
            ptr = NULL;

            free(n);
            n = NULL;

            free(str);
            str = NULL;
        }

        status = printResult(status);

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with value and mix malloc [5x]: ");

        q = initQueue();

        for (i = 0; i < 5; i++)
        {
            if (i & 1)
            {
                str = "test string";
                enqueue(q, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
                memset(str, '\0', (strlen("test string") + 3));
                sprintf(str, "test string %d", i);
                enqueue(q, str, TRUE);
            }
        }

        i = -1;

        while ((n = dequeue(q, &ptr, &isMalloc)) && status)
        {
            if (++i & 1)
            {
                if (n && ! strcmp((char*)ptr, "test string") && ! isMalloc)
                {
                    status = TRUE;
                }
                else
                {
                    status = FALSE;
                }
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
                memset(str, '\0', (strlen("test string") + 3));
                sprintf(str, "test string %d", i);

                if (! strcmp((char*)ptr, str) && isMalloc)
                {
                    status = TRUE;
                }
                else
                {
                    status = FALSE;
                }

                free(ptr);
                ptr = NULL;

                free(str);
                str = NULL;
            }

            free(n);
            n = NULL;
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;

            if (n -> isMalloc)
            {
                free(n -> value);
                n -> value = NULL;
            }

            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with a value and not malloc [100000x]: ");
        q = initQueue();

        for (i = 0; i < 100000; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        i = 0;

        while ((n = dequeue(q, &ptr, &isMalloc)) && status)
        {
            if (n && ! strcmp((char*)ptr, "test string") && ! isMalloc)
            {
                status = TRUE;
            }
            else
            {
                status = FALSE;
            }

            free(n);
            n = NULL;
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;
            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with a value and is malloc [100000x]: ");
        q = initQueue();

        for (i = 0; i < 100000; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        i = -1;

        while ((n = dequeue(q, &ptr, &isMalloc)) && status)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", ++i);

            if (! strcmp((char*)ptr, str) && isMalloc)
            {
                status = TRUE;
            }
            else
            {
                status = FALSE;
            }

            free(ptr);
            ptr = NULL;

            free(n);
            n = NULL;

            free(str);
            str = NULL;
        }

        status = printResult(status);

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with value and mix malloc [100000x]: ");

        q = initQueue();

        for (i = 0; i < 100000; i++)
        {
            if (i & 1)
            {
                str = "test string";
                enqueue(q, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
                memset(str, '\0', (strlen("test string") + 9));
                sprintf(str, "test string %d", i);
                enqueue(q, str, TRUE);
            }
        }

        i = -1;

        while ((n = dequeue(q, &ptr, &isMalloc)) && status)
        {
            if (++i & 1)
            {
                if (n && ! strcmp((char*)ptr, "test string") && ! isMalloc)
                {
                    status = TRUE;
                }
                else
                {
                    status = FALSE;
                }
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
                memset(str, '\0', (strlen("test string") + 9));
                sprintf(str, "test string %d", i);

                if (! strcmp((char*)ptr, str) && isMalloc)
                {
                    status = TRUE;
                }
                else
                {
                    status = FALSE;
                }

                free(ptr);
                ptr = NULL;

                free(str);
                str = NULL;
            }

            free(n);
            n = NULL;
        }

        status = printResult(status);

        while (q -> head != NULL)
        {
            n = q -> head;
            q -> head = q -> head -> next;

            if (n -> isMalloc)
            {
                free(n -> value);
                n -> value = NULL;
            }

            free(n);
            n = NULL;
        }

        free(q);
        q = NULL;
    }

    return status;
}

int testQueuePeek()
{
    Queue* q;

    void* voidPtr;
    int ret;
    int i;

    int status = TRUE;

    voidPtr = NULL;
    q = NULL;

    header("Queue Peek");

    if (status)
    {
        fprintf(stdout, "Testing peek() on an empty queue: ");
        q = initList();
        peekFirst(q, &voidPtr, &ret);
        status = printResult(! voidPtr &&
                             ret == -1);
        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing peek() on a populated queue: ");
        q = initList();
        i = 5;
        insertFirst(q, &i, FALSE);
        peek(q, &voidPtr, &ret);
        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == i &&
                             ! ret);

        voidPtr = NULL;

        free(q -> head);
        free(q);
        q = NULL;
    }

    return status;
}

int testQueueClear()
{
    Queue* q = NULL;
    char* str = NULL;
    int i;
    int status = TRUE;

    header("Queue Clear");

    if (status)
    {
        fprintf(stdout, "Testing enqueue() without a value: ");
        q = initQueue();
        enqueue(q, NULL, FALSE);
        clearList(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout, "Testing enqueue() with a value and not malloc: ");
        q = initQueue();
        str = "test string";
        enqueue(q, str, FALSE);
        clearList(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout, "Testing enqueue() with a value and malloc: ");
        q = initQueue();
        str = (char*)malloc(sizeof(char) * (strlen("test string") + 1));
        strncpy(str, "test string", strlen("test string") + 1);
        enqueue(q, str, TRUE);

        clearList(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with a value and not malloc [5x]: ");
        q = initQueue();

        for (i = 0; i < 5; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        clearList(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with a value and is malloc [5x]: ");
        q = initQueue();

        for (i = 0; i < 5; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        clearList(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with value and mix malloc [5x]: ");

        q = initQueue();

        for (i = 0; i < 5; i++)
        {
            if (i & 1)
            {
                str = "test string";
                enqueue(q, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
                memset(str, '\0', (strlen("test string") + 3));
                sprintf(str, "test string %d", i);
                enqueue(q, str, TRUE);
            }
        }

        clearList(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with a value and not malloc [100000x]: ");
        q = initQueue();

        for (i = 0; i < 100000; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        clearList(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with a value and is malloc [100000x]: ");
        q = initQueue();

        for (i = 0; i < 100000; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        clearList(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing enqueue() with value and mix malloc [100000x]: ");

        q = initQueue();

        for (i = 0; i < 100000; i++)
        {
            if (i & 1)
            {
                str = "test string";
                enqueue(q, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
                memset(str, '\0', (strlen("test string") + 9));
                sprintf(str, "test string %d", i);
                enqueue(q, str, TRUE);
            }
        }

        clearList(&q);
        status = printResult(! q);
    }

    return status;
}

int printResult(int cond)
{
    char* green;
    char* red;
    char* reset;

    green = "\033[32m";
    red = "\033[31m";
    reset = "\033[0m";

    if (cond)
    {
        fprintf(stdout, "%s%s%s\n", green, "Passed", reset);
    }
    else
    {
        fprintf(stdout, "%s%s%s\n", red, "Failed", reset);
    }

    return cond;
}

void header(char* msg)
{
    char* line = NULL;
    int len = strlen(msg);

    line = (char*)malloc(sizeof(char) * (len + 1));
    memset(line, '\0', len + 1);
    memset(line, '=', len);
    fprintf(stdout, "%s\n%s\n%s\n", line, msg, line);

    free(line);
    line = NULL;
}
