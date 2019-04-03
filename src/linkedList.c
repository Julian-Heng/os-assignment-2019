/**
 * Filename: linkedList.c
 * Author:   Juilan Heng (19473701)
 * Purpose:  Provide the initialisation of a doubly linked double ended
 *           linked list as well as several utility functions.
 *
 * Last Modified: 2018-10-20T15:02:03+08:00
 **/

#include <stdlib.h>
#include "linkedList.h"

#define FALSE 0
#define TRUE !FALSE

/* Static functions */
static void freeNode(LinkedListNode** node);
static void clearListRecurse(LinkedListNode** node);
static void setNextToNode(LinkedListNode* node, LinkedListNode* nextNode);
static void setPrevToNode(LinkedListNode* node, LinkedListNode* prevNode);
static void setNextInList(LinkedListNode* node);
static void setPrevInList(LinkedListNode* node);

LinkedListNode* initNode(void* newValue, int isMalloc)
{
    LinkedListNode* newNode;

    /* Allocate memory in the heap for a linked list node */
    newNode = (LinkedListNode*)malloc(sizeof(LinkedListNode));

    /* Check if malloc failed */
    if (newNode)
    {
        /* Set default values */
        newNode -> next = NULL;
        newNode -> prev = NULL;
        newNode -> value = newValue;
        newNode -> isMalloc = isMalloc;
    }

    return newNode;
}

LinkedList* initList(void)
{
    LinkedList* newList;

    /* Allocate memory in the heap for a linked list */
    newList = (LinkedList*)malloc(sizeof(LinkedList));

    /* Check if malloc failed */
    if (newList)
    {
        /* Set default values */
        newList -> head = NULL;
        newList -> tail = NULL;
        newList -> length = 0;
    }

    return newList;
}

void insertFirst(LinkedList* list, void* newValue, int isMalloc)
{
    LinkedListNode* newNode;

    /* Create a new node with newValue */
    newNode = initNode(newValue, isMalloc);

    /* Special case for empty list */
    if (list && isEmpty(list))
    {
        list -> head = newNode;
        list -> tail = newNode;
    }
    else
    {
        /**
         * Set the previous pointer in the current head to newNode,
         * then set the next in newNode to current head.
         * Set the head to newNode
         **/
        setPrevToNode(list -> head, newNode);
        setNextToNode(newNode, list -> head);
        list -> head = newNode;
    }

    list -> length += 1;
}

void insertLast(LinkedList* list, void* newValue, int isMalloc)
{
    LinkedListNode* newNode;

    /* Create a new node with newValue */
    newNode = initNode(newValue, isMalloc);

    /* Special case for empty list */
    if (list && isEmpty(list))
    {
        list -> head = newNode;
        list -> tail = newNode;
    }
    else
    {
        /**
         * Set the previous pointer in newNode to point to the
         * current tail, then set the next in current tail to
         * newNode. Then set newNode as the new tail
         **/
        setPrevToNode(newNode, list -> tail);
        setNextToNode(list -> tail, newNode);
        list -> tail = newNode;
    }

    list -> length += 1;
}

LinkedListNode* removeFirst(LinkedList* list)
{
    LinkedListNode* remove;

    remove = NULL;

    /* Check if list is not null and not empty */
    if (list && ! isEmpty(list))
    {
        /* Get the first node */
        remove = list -> head;

        /* Special case for a single item linked list */
        if (list -> head == list -> tail)
        {
            list -> head = NULL;
            list -> tail = NULL;
        }
        else
        {
            setNextInList(list -> head);
        }

        list -> length -= 1;
    }

    return remove;
}

LinkedListNode* removeLast(LinkedList* list)
{
    LinkedListNode* remove;

    remove = NULL;

    /* Check if list is not null and not empty */
    if (list && ! isEmpty(list))
    {
        /* Get the last node */
        remove = list -> tail;

        /* Special case for a single item linked list */
        if (list -> head == list -> tail)
        {
            list -> head = NULL;
            list -> tail = NULL;
        }
        else
        {
            setPrevInList(list -> tail);
        }

        list -> length -= 1;
    }

    return remove;
}

void* peekFirst(LinkedList* list)
{
    return (list && ! isEmpty(list)) ? list -> head -> value : NULL;
}

void* peekLast(LinkedList* list)
{
    return (list && ! isEmpty(list)) ? list -> tail -> value : NULL;
}

int getListLength(LinkedList* list)
{
    return list -> length;
}

void clearList(LinkedList** list)
{
    if (*list)
    {
        if (! isEmpty(*list))
        {
            clearListRecurse(&((*list) -> head));
        }

        free(*list);
        *list = NULL;
    }
}

static void clearListRecurse(LinkedListNode** node)
{
    if (*node)
    {
        clearListRecurse(&((*node) -> next));
        freeNode(node);
    }
}

static void freeNode(LinkedListNode** node)
{
    if ((*node) -> isMalloc && (*node) -> value)
    {
        free((*node) -> value);
        (*node) -> value = NULL;
    }

    free(*node);
    *node = NULL;
}

static void setNextToNode(LinkedListNode* node, LinkedListNode* nextNode)
{
    /* If node and nextNode is not null */
    if (node && nextNode)
    {
        node -> next = nextNode;
    }
}

static void setPrevToNode(LinkedListNode* node, LinkedListNode* prevNode)
{
    /* If node and prevNode is not null */
    if (node && prevNode)
    {
        node -> prev = prevNode;
    }
}

static void setNextInList(LinkedListNode* node)
{
    /* If node is not null */
    if (node)
    {
        node = node -> next;
    }
}

static void setPrevInList(LinkedListNode* node)
{
    /* If node is not null */
    if (node)
    {
        node = node -> prev;
    }
}

int isEmpty(LinkedList* list)
{
    return (list &&
            (! (list -> head)) &&
            (! (list -> tail)) &&
            list -> length == 0) ? TRUE : FALSE;
}
