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
        q = initQueue(5);
        status = printResult(!! q &&
                             isQueueEmpty(q) &&
                             getQueueLength(q) == 0 &&
                             getQueueMaxLength(q) == 5);
        free(q->link);
        q->link = NULL;

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
        fprintf(stdout, "Testing clear() without a value: ");
        q = initQueue(5);
        enqueue(q, NULL, FALSE);
        status = printResult(!! q &&
                             getQueueLength(q) == 1 &&
                             getQueueMaxLength(q) == 5 &&
                             ! (q->link->head->isMalloc) &&
                             ! (q->link->head->value));

        free(q->link->head);
        q->link->head = NULL;

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing clear() with a value and not malloc: ");
        q = initQueue(5);
        str = "test string";
        enqueue(q, str, FALSE);
        status = printResult(!! q &&
                             getQueueLength(q) == 1 &&
                             getQueueMaxLength(q) == 5 &&
                             ! (q->link->head->isMalloc) &&
                             ! strcmp(q->link->head->value, "test string"));

        free(q->link->head);
        q->link->head = NULL;

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing clear() with a value and malloc: ");
        q = initQueue(5);
        str = (char*)malloc(sizeof(char) * (strlen("test string") + 1));
        strncpy(str, "test string", strlen("test string") + 1);
        enqueue(q, str, TRUE);
        status = printResult(!! q &&
                             getQueueLength(q) == 1 &&
                             getQueueMaxLength(q) == 5 &&
                             q->link->head->isMalloc &&
                             ! strcmp(q->link->head->value, "test string"));

        free(q->link->head->value);
        q->link->head->value = NULL;

        free(q->link->head);
        q->link->head = NULL;

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing clear() with a full queue: ");
        q = initQueue(2);
        status = enqueue(q, NULL, FALSE);
        status = enqueue(q, NULL, FALSE);
        status = printResult(! enqueue(q, NULL, FALSE));

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;
            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with a value and not malloc [5x]: ");
        q = initQueue(5);

        for (i = 0; i < 5; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        i = 0;
        n = q->link->head;

        while (n != NULL && i++ < 5 && status)
        {
            if (!! q && ! strcmp(n->value, "test string"))
            {
                status = TRUE;
                n = n->next;
            }
            else
            {
                status = FALSE;
            }
        }

        status = printResult(status);

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;
            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with a value and is malloc [5x]: ");
        q = initQueue(5);

        for (i = 0; i < 5; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        i = -1;
        n = q->link->head;

        while (n != NULL && i++ < 5 && status)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);

            if (!! q && ! strcmp(n->value, str))
            {
                status = TRUE;
                n = n->next;
            }
            else
            {
                status = FALSE;
            }

            free(str);
            str = NULL;
        }

        status = printResult(status);

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;

            free(n->value);
            n->value = NULL;

            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with value and mix malloc [5x]: ");

        q = initQueue(5);

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
        n = q->link->head;

        while (n != NULL && i++ < 5 && status)
        {
            if (i & 1)
            {
                if (!! q && ! strcmp(n->value, "test string"))
                {
                    status = TRUE;
                    n = n->next;
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

                if (!! q && ! strcmp(n->value, str))
                {
                    status = TRUE;
                    n = n->next;
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

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;

            if (n->isMalloc)
            {
                free(n->value);
                n->value = NULL;
            }

            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with a value and not malloc [100000x]: ");
        q = initQueue(100000);

        for (i = 0; i < 100000; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        i = 0;
        n = q->link->head;

        while (n != NULL && i++ < 100000 && status)
        {
            if (!! q && ! strcmp(n->value, "test string"))
            {
                status = TRUE;
                n = n->next;
            }
            else
            {
                status = FALSE;
            }
        }

        status = printResult(status);

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;
            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with a value and is malloc [100000x]: ");
        q = initQueue(100000);

        for (i = 0; i < 100000; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        i = -1;
        n = q->link->head;

        while (n != NULL && i++ < 100000 && status)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);

            if (!! q && ! strcmp(n->value, str))
            {
                status = TRUE;
                n = n->next;
            }
            else
            {
                status = FALSE;
            }

            free(str);
            str = NULL;
        }

        status = printResult(status);

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;

            free(n->value);
            n->value = NULL;

            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with value and mix malloc [100000x]: ");

        q = initQueue(100000);

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
        n = q->link->head;

        while (n != NULL && i++ < 100000 && status)
        {
            if (i & 1)
            {
                if (!! q && ! strcmp(n->value, "test string"))
                {
                    status = TRUE;
                    n = n->next;
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

                if (!! q && ! strcmp(n->value, str))
                {
                    status = TRUE;
                    n = n->next;
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

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;

            if (n->isMalloc)
            {
                free(n->value);
                n->value = NULL;
            }

            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

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
        q = initQueue(5);
        n = dequeue(q, &ptr, &isMalloc);
        status = printResult(!! q &&
                             ! n &&
                             ! ptr &&
                             isMalloc == -1);

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing dequeue() without a value: ");
        q = initQueue(5);
        enqueue(q, NULL, FALSE);
        n = dequeue(q, &ptr, &isMalloc);
        status = printResult(!! q &&
                             n &&
                             ! ptr &&
                             ! isMalloc);

        free(n);
        n = NULL;

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing dequeue() with a value and not malloc: ");
        q = initQueue(5);
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

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing dequeue() with a value and malloc: ");
        q = initQueue(5);
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

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with a value and not malloc [5x]: ");
        q = initQueue(5);

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

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;
            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with a value and is malloc [5x]: ");
        q = initQueue(5);

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

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with value and mix malloc [5x]: ");

        q = initQueue(5);

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

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;

            if (n->isMalloc)
            {
                free(n->value);
                n->value = NULL;
            }

            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with a value and not malloc [100000x]: ");
        q = initQueue(100000);

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

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;
            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with a value and is malloc [100000x]: ");
        q = initQueue(100000);

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

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing dequeue() with value and mix malloc [100000x]: ");

        q = initQueue(100000);

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

        while (q->link->head != NULL)
        {
            n = q->link->head;
            q->link->head = q->link->head->next;

            if (n->isMalloc)
            {
                free(n->value);
                n->value = NULL;
            }

            free(n);
            n = NULL;
        }

        free(q->link);
        q->link = NULL;

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
        q = initQueue(5);
        peek(q, &voidPtr, &ret);
        status = printResult(! voidPtr &&
                             ret == -1);

        free(q->link);
        q->link = NULL;

        free(q);
        q = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing peek() on a populated queue: ");
        q = initQueue(5);
        i = 5;
        enqueue(q, &i, FALSE);
        peek(q, &voidPtr, &ret);
        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == i &&
                             ! ret);

        voidPtr = NULL;

        free(q->link->head);
        q->link->head = NULL;

        free(q->link);
        q->link = NULL;

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
        fprintf(stdout, "Testing clear() without a value: ");
        q = initQueue(5);
        enqueue(q, NULL, FALSE);
        clearQueue(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout, "Testing clear() with a value and not malloc: ");
        q = initQueue(5);
        str = "test string";
        enqueue(q, str, FALSE);
        clearQueue(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout, "Testing clear() with a value and malloc: ");
        q = initQueue(5);
        str = (char*)malloc(sizeof(char) * (strlen("test string") + 1));
        strncpy(str, "test string", strlen("test string") + 1);
        enqueue(q, str, TRUE);

        clearQueue(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with a value and not malloc [5x]: ");
        q = initQueue(5);

        for (i = 0; i < 5; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        clearQueue(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with a value and is malloc [5x]: ");
        q = initQueue(5);

        for (i = 0; i < 5; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        clearQueue(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with value and mix malloc [5x]: ");

        q = initQueue(5);

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

        clearQueue(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with a value and not malloc [100000x]: ");
        q = initQueue(100000);

        for (i = 0; i < 100000; i++)
        {
            str = "test string";
            enqueue(q, str, FALSE);
            str = "";
        }

        clearQueue(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with a value and is malloc [100000x]: ");
        q = initQueue(100000);

        for (i = 0; i < 100000; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);
            enqueue(q, str, TRUE);
        }

        clearQueue(&q);
        status = printResult(! q);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clear() with value and mix malloc [100000x]: ");

        q = initQueue(100000);

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

        clearQueue(&q);
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
