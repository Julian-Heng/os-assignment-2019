#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../linkedList.h"
#include "test_linkedList.h"

#define FALSE 0
#define TRUE !FALSE

int main(void)
{
    int count = 0;
    int status = TRUE;
    int (*funcs[5])(void) = {
        testListConstructor,
        testListInsert,
        testListRemove,
        testListPeek,
        testListClear
    };

    while (status && count < 5)
    {
        status = (*funcs[count++])();
    }

    return status == TRUE ? FALSE : TRUE;
}

int testListConstructor(void)
{
    LinkedList* list;
    LinkedListNode* node;
    char* str;

    int status = TRUE;

    list = NULL;
    node = NULL;
    str = NULL;

    header("LinkedList Constructor");

    if (status)
    {
        fprintf(stdout, "Testing initNode() with no value and not malloc: ");
        node = initNode(NULL, FALSE);
        status = printResult(!! node && ! (node -> isMalloc));
        free(node);
        node = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing initNode() with no value and is malloc: ");
        node = initNode(NULL, TRUE);
        status = printResult(!! node && node -> isMalloc);
        free(node);
        node = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing initNode() with value and not malloc: ");
        str = "test string";
        node = initNode(str, FALSE);
        status = printResult(!! node &&
                             ! (node -> isMalloc) &&
                             ! strcmp(node -> value, "test string"));
        str = "";
        free(node);
        node = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing initNode() with value and malloc: ");
        str = (char*)malloc(sizeof(char) * (strlen("test string") + 1));
        strncpy(str, "test string", strlen("test string") + 1);
        node = initNode(str, TRUE);
        status = printResult(!! node &&
                             (node -> isMalloc) &&
                             ! strcmp(node -> value, "test string"));
        free(str);
        str = NULL;

        free(node);
        node = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing initList() with no nodes: ");
        list = initList();
        status = printResult(!! list && list -> length == 0);
        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing initList() with isListEmpty(): ");
        list = initList();
        status = printResult(!! list && isListEmpty(list));
        free(list);
        list = NULL;
    }

    return status;
}

int testListInsert(void)
{
    LinkedList* list;
    LinkedListNode* node;
    char* str;
    int i;

    int status = TRUE;

    list = NULL;
    node = NULL;
    str = NULL;

    header("LinkedList Insert");

    if (status)
    {
        fprintf(stdout,
                "Testing insertFirst() without value and not malloc: ");

        list = initList();
        insertFirst(list, NULL, FALSE);

        status = printResult(!! list &&
                             list -> length == 1 &&
                             list -> head &&
                             list -> tail &&
                             list -> head == list -> tail &&
                             ! list -> head -> value);

        free(list -> head);
        list -> head = NULL;

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing insertFirst() with value and not malloc: ");

        list = initList();
        str = "test string";
        insertFirst(list, str, FALSE);

        status = printResult(!! list &&
                             list -> length == 1 &&
                             list -> head &&
                             list -> tail &&
                             list -> head == list -> tail &&
                             list -> head -> value &&
                             ! list -> head -> isMalloc &&
                             ! strcmp(list -> head -> value, "test string"));

        str = "";
        free(list -> head);
        list -> head = NULL;

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing insertFirst() with value and is malloc: ");

        list = initList();
        str = (char*)malloc(sizeof(char) * (strlen("test string") + 1));
        strncpy(str, "test string", strlen("test string") + 1);

        insertFirst(list, str, TRUE);
        status = printResult(!! list &&
                             list -> length == 1 &&
                             list -> head &&
                             list -> tail &&
                             list -> head == list -> tail &&
                             list -> head -> value &&
                             list -> head -> isMalloc &&
                             ! strcmp(list -> head -> value, "test string"));

        free(str);
        str = NULL;

        free(list -> head);
        list -> head = NULL;

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing insertFirst() without value and not malloc [5x]: ");

        list = initList();

        for (i = 0; i < 5; i++)
        {
            insertFirst(list, NULL, FALSE);
        }

        i = 0;

        while (node != NULL && i++ < 5 && status)
        {
            if (!! list && list -> length == 5 && ! list -> head -> value)
            {
                status = TRUE;
                node = node -> next;
            }
            else
            {
                status = FALSE;
            }
        }

        status = printResult(status);

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;
            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing insertFirst() with value and not malloc [5x]: ");

        list = initList();

        for (i = 0; i < 5; i++)
        {
            str = "test string";
            insertFirst(list, str, FALSE);
            str = "";
        }

        i = 0;

        while (node != NULL && i++ < 5 && status)
        {
            if (!! list &&
                list -> length == 5 &&
                ! node -> isMalloc &&
                ! strcmp(node -> value, "test string"))
            {
                status = TRUE;
                node = node -> next;
            }
            else
            {
                status = FALSE;
            }
        }

        status = printResult(status);

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;
            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing insertFirst() with value and is malloc [5x]: ");

        list = initList();

        for (i = 0; i < 5; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);
            insertFirst(list, str, TRUE);
        }

        i = 0;

        while (node != NULL && i++ < 5 && status)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);

            if (!! list &&
                list -> length == 5 &&
                node -> isMalloc &&
                ! strcmp(node -> value, str))
            {
                status = TRUE;
                node = node -> next;
            }
            else
            {
                status = FALSE;
            }

            free(str);
            str = NULL;
        }

        status = printResult(status);

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;

            free(node -> value);
            node -> value = NULL;

            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing insertFirst() with value and mix malloc [5x]: ");

        list = initList();

        for (i = 0; i < 5; i++)
        {
            if (i & 1)
            {
                str = "test string";
                insertFirst(list, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
                memset(str, '\0', (strlen("test string") + 3));
                sprintf(str, "test string %d", i);
                insertFirst(list, str, TRUE);
            }
        }

        i = 0;

        while (node != NULL && i++ < 5 && status)
        {
            if (i & 1)
            {
                if (!! list &&
                    list -> length == 5 &&
                    ! node -> isMalloc &&
                    ! strcmp(node -> value, "test string"))
                {
                    status = TRUE;
                    node = node -> next;
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

                if (!! list &&
                    list -> length == 5 &&
                    node -> isMalloc &&
                    ! strcmp(node -> value, str))
                {
                    status = TRUE;
                    node = node -> next;
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

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;

            if (node -> isMalloc)
            {
                free(node -> value);
                node -> value = NULL;
            }

            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing insertFirst() without value and not malloc [100000x]: ");

        list = initList();

        for (i = 0; i < 100000; i++)
        {
            insertFirst(list, NULL, FALSE);
        }

        i = 0;

        while (node != NULL && i++ < 100000 && status)
        {
            if (!! list && list -> length == 100000 && ! list -> head -> value)
            {
                status = TRUE;
                node = node -> next;
            }
            else
            {
                status = FALSE;
            }
        }

        status = printResult(status);

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;
            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing insertFirst() with value and not malloc [100000x]: ");

        list = initList();

        for (i = 0; i < 100000; i++)
        {
            str = "test string";
            insertFirst(list, str, FALSE);
            str = "";
        }

        i = 0;

        while (node != NULL && i++ < 100000 && status)
        {
            if (!! list &&
                list -> length == 100000 &&
                ! node -> isMalloc &&
                ! strcmp(node -> value, "test string"))
            {
                status = TRUE;
                node = node -> next;
            }
            else
            {
                status = FALSE;
            }
        }

        status = printResult(status);

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;
            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
        str = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing insertFirst() with value and is malloc [100000x]: ");

        list = initList();

        for (i = 0; i < 100000; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);
            insertFirst(list, str, TRUE);
        }

        i = 0;

        while (node != NULL && i++ < 100000 && status)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);

            if (!! list &&
                list -> length == 100000 &&
                node -> isMalloc &&
                ! strcmp(node -> value, str))
            {
                status = TRUE;
                node = node -> next;
            }
            else
            {
                status = FALSE;
            }

            free(str);
            str = NULL;
        }

        status = printResult(status);

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;

            free(node -> value);
            node -> value = NULL;

            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout,
                "Testing insertFirst() with value and mix malloc [100000x]: ");

        list = initList();

        for (i = 0; i < 100000; i++)
        {
            if (i & 1)
            {
                str = "test string";
                insertFirst(list, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
                memset(str, '\0', (strlen("test string") + 9));
                sprintf(str, "test string %d", i);
                insertFirst(list, str, TRUE);
            }
        }

        i = 0;

        while (node != NULL && i++ < 100000 && status)
        {
            if (i & 1)
            {
                if (!! list &&
                    list -> length == 100000 &&
                    ! node -> isMalloc &&
                    ! strcmp(node -> value, "test string"))
                {
                    status = TRUE;
                    node = node -> next;
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

                if (!! list &&
                    list -> length == 100000 &&
                    node -> isMalloc &&
                    ! strcmp(node -> value, str))
                {
                    status = TRUE;
                    node = node -> next;
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

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;

            if (node -> isMalloc)
            {
                free(node -> value);
                node -> value = NULL;
            }

            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    return status;
}

int testListRemove(void)
{
    LinkedList* list;
    LinkedListNode* node;

    void* voidPtr;

    int ret;
    int i;
    int* j;

    int status = TRUE;

    voidPtr = NULL;
    list = NULL;
    node = NULL;
    ret = 0;

    header("LinkedList Remove");

    if (status)
    {
        fprintf(stdout, "Testing removeFirst() on an empty list: ");
        list = initList();
        removeFirst(list, &voidPtr, &ret);
        status = printResult(! voidPtr && ret == -1);
        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing removeFirst() on a single node list: ");
        list = initList();
        i = 5;
        insertFirst(list, &i, FALSE);
        node = removeFirst(list, &voidPtr, &ret);
        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == i &&
                             ! ret);

        free(node);
        node = NULL;

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing removeFirst() on a multiple node list: ");
        list = initList();

        for (i = 0; i < 5; i++)
        {
            j = (int*)malloc(sizeof(int));
            *j = i;
            insertLast(list, j, TRUE);
        }

        node = removeFirst(list, &voidPtr, &ret);

        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == 0 &&
                             ret);

        free(voidPtr);
        voidPtr = NULL;

        free(node);
        node = NULL;

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;

            if (node -> isMalloc)
            {
                free(node -> value);
                node -> value = NULL;
            }

            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing removeLast() on an empty list: ");
        list = initList();
        removeLast(list, &voidPtr, &ret);
        status = printResult(! voidPtr && ret == -1);
        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing removeLast() on a single node list: ");
        list = initList();
        i = 5;
        insertLast(list, &i, FALSE);
        node = removeLast(list, &voidPtr, &ret);
        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == i &&
                             ! ret);

        free(node);
        node = NULL;

        free(list -> head);
        node = NULL;

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing removeLast() on a multiple node list: ");
        list = initList();

        for (i = 0; i < 5; i++)
        {
            j = (int*)malloc(sizeof(int));
            *j = i;
            insertLast(list, j, TRUE);
        }

        removeLast(list, &voidPtr, &ret);
        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == 4 &&
                             ret);

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;

            if (node -> isMalloc)
            {
                free(node -> value);
                node -> value = NULL;
            }

            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    return status;
}

int testListPeek(void)
{
    LinkedList* list;
    LinkedListNode* node;

    void* voidPtr;
    int ret;
    int i;
    int* j;

    int status = TRUE;

    voidPtr = NULL;
    list = NULL;
    node = NULL;

    header("LinkedList Peek");

    if (status)
    {
        fprintf(stdout, "Testing peekFirst() on an empty list: ");
        list = initList();
        peekFirst(list, &voidPtr, &ret);
        status = printResult(! voidPtr &&
                             ret == -1);
        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing peekFirst() on a single node list: ");
        list = initList();
        i = 5;
        insertFirst(list, &i, FALSE);
        peekFirst(list, &voidPtr, &ret);
        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == i &&
                             ! ret);

        voidPtr = NULL;

        free(list -> head);
        node = NULL;

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing peekFirst() on a multiple node list: ");
        list = initList();

        for (i = 0; i < 5; i++)
        {
            j = (int*)malloc(sizeof(int));
            *j = i;
            insertLast(list, j, TRUE);
        }

        peekFirst(list, &voidPtr, &ret);
        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == 0 &&
                             ret);

        voidPtr = NULL;

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;

            if (node -> isMalloc)
            {
                free(node -> value);
                node -> value = NULL;
            }

            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing peekLast() on an empty list: ");
        list = initList();
        peekLast(list, &voidPtr, &ret);
        status = printResult(! voidPtr &&
                             ret == -1);
        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing peekLast() on a single node list: ");
        list = initList();
        i = 5;
        insertLast(list, &i, FALSE);
        peekLast(list, &voidPtr, &ret);
        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == i &&
                             ! ret);

        voidPtr = NULL;

        free(list -> head);
        node = NULL;

        free(list);
        list = NULL;
    }

    if (status)
    {
        fprintf(stdout, "Testing peekLast() on a multiple node list: ");
        list = initList();

        for (i = 0; i < 5; i++)
        {
            j = (int*)malloc(sizeof(int));
            *j = i;
            insertLast(list, j, TRUE);
        }

        peekLast(list, &voidPtr, &ret);
        status = printResult(!! voidPtr &&
                             *(int*)(voidPtr) == 4 &&
                             ret);

        voidPtr = NULL;

        while (list -> head != NULL)
        {
            node = list -> head;
            list -> head = list -> head -> next;

            if (node -> isMalloc)
            {
                free(node -> value);
                node -> value = NULL;
            }

            free(node);
            node = NULL;
        }

        free(list);
        list = NULL;
    }

    return status;
}

int testListClear(void)
{
    LinkedList* list;
    char* str;
    int i;

    int status = TRUE;

    list = NULL;
    str = NULL;

    header("LinkedList Clear");

    if (status)
    {
        fprintf(stdout, "Testing clearList() on an empty list: ");

        list = initList();
        clearList(&list);

        status = printResult(! list);
    }

    if (status)
    {
        fprintf(stdout, "Testing clearList() with value and not malloc: ");

        list = initList();
        str = "test string";
        insertFirst(list, str, FALSE);
        clearList(&list);

        status = printResult(! list);
    }

    if (status)
    {
        fprintf(stdout, "Testing clearList() with value and is malloc: ");

        list = initList();
        str = (char*)malloc(sizeof(char) * (strlen("test string") + 1));
        strncpy(str, "test string", strlen("test string") + 1);

        insertFirst(list, str, TRUE);
        clearList(&list);

        status = printResult(! list);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clearList() with value and not malloc [5x]: ");

        list = initList();

        for (i = 0; i < 5; i++)
        {
            str = "test string";
            insertFirst(list, str, FALSE);
            str = "";
        }

        clearList(&list);

        status = printResult(! list);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clearList() with value and is malloc [5x]: ");

        list = initList();

        for (i = 0; i < 5; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
            memset(str, '\0', (strlen("test string") + 3));
            sprintf(str, "test string %d", i);
            insertFirst(list, str, TRUE);
        }

        clearList(&list);

        status = printResult(! list);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clearList() with value and mix malloc [5x]: ");

        list = initList();

        for (i = 0; i < 5; i++)
        {
            if (i & 1)
            {
                str = "test string";
                insertFirst(list, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 3));
                memset(str, '\0', (strlen("test string") + 3));
                sprintf(str, "test string %d", i);
                insertFirst(list, str, TRUE);
            }
        }

        clearList(&list);

        status = printResult(! list);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clearList() with value and not malloc [100000x]: ");

        list = initList();

        for (i = 0; i < 100000; i++)
        {
            str = "test string";
            insertFirst(list, str, FALSE);
            str = "";
        }

        clearList(&list);

        status = printResult(! list);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clearList() with value and is malloc [100000x]: ");

        list = initList();

        for (i = 0; i < 100000; i++)
        {
            str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
            memset(str, '\0', (strlen("test string") + 9));
            sprintf(str, "test string %d", i);
            insertFirst(list, str, TRUE);
        }

        clearList(&list);

        status = printResult(! list);
    }

    if (status)
    {
        fprintf(stdout,
                "Testing clearList() with value and mix malloc [100000x]: ");

        list = initList();

        for (i = 0; i < 100000; i++)
        {
            if (i & 1)
            {
                str = "test string";
                insertFirst(list, str, FALSE);
                str = "";
            }
            else
            {
                str = (char*)malloc(sizeof(char) * (strlen("test string") + 9));
                memset(str, '\0', (strlen("test string") + 9));
                sprintf(str, "test string %d", i);
                insertFirst(list, str, TRUE);
            }
        }

        clearList(&list);

        status = printResult(! list);
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
