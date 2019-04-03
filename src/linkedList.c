/**
 * Filename: linkedList.c
 * Author:   Juilan Heng (19473701)
 * Purpose:  Provide the initialisation of a doubly linked double ended
 *           linked list as well as several utility functions.
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
    if ((newNode = (LinkedListNode*)malloc(sizeof(LinkedListNode))))
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
    if ((newList = (LinkedList*)malloc(sizeof(LinkedList))))
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
    if (list && isListEmpty(list))
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

    (list -> length)++;
}

void insertLast(LinkedList* list, void* newValue, int isMalloc)
{
    LinkedListNode* newNode;

    /* Create a new node with newValue */
    newNode = initNode(newValue, isMalloc);

    /* Special case for empty list */
    if (list && isListEmpty(list))
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

    (list -> length)++;
}

/**
 * Remove the first node in the list We return a pointer to a node because we
 * cannot free it within this function. Thus the calling function will need to
 * perform the freeing of the node. If we free the node within this function,
 * we loose access to the void pointer within the node
 **/
LinkedListNode* removeFirst(LinkedList* list, void** voidPtr, int* isMalloc)
{
    LinkedListNode* node = NULL;
    peekFirst(list, voidPtr, isMalloc);

    if (list && ! isListEmpty(list))
    {
        node = list -> head;
        if (list -> head == list -> tail)
        {
            list -> head = NULL;
            list -> tail = NULL;
        }
        else
        {
            setNextInList(list -> head);
        }

        (list -> length)--;
    }

    return node;
}

/**
 * Same as removeFirst(), except we do it to the last node in the list
 **/
LinkedListNode* removeLast(LinkedList* list, void** voidPtr, int* isMalloc)
{
    LinkedListNode* node = NULL;
    peekLast(list, voidPtr, isMalloc);

    if (list && ! isListEmpty(list))
    {
        node = list -> tail;
        if (list -> head == list -> tail)
        {
            list -> head = NULL;
            list -> tail = NULL;
        }
        else
        {
            setPrevInList(list -> tail);
        }

        (list -> length)--;
    }

    return node;
}

/**
 * Getting the value in the first node without removing it from the list
 * voidPtr is where the value is stored and isMalloc is a boolean that
 * determines if it is stored on the heap or stack
 **/
void peekFirst(LinkedList* list, void** voidPtr, int* isMalloc)
{
    if (list && ! isListEmpty(list))
    {
        *voidPtr = list -> head -> value;
        *isMalloc = list -> head -> isMalloc;
    }
    else
    {
        *voidPtr = NULL;
        *isMalloc = -1;
    }
}

/**
 * Same as peekFirst() except it peeks the end of the list
 **/
void peekLast(LinkedList* list, void** voidPtr, int* isMalloc)
{
    if (list && ! isListEmpty(list))
    {
        *voidPtr = list -> tail -> value;
        *isMalloc = list -> tail -> isMalloc;
    }
    else
    {
        *voidPtr = NULL;
        *isMalloc = -1;
    }
}

int getListLength(LinkedList* list)
{
    return list -> length;
}

void clearList(LinkedList** list)
{
    if (*list)
    {
        if (! isListEmpty(*list))
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

int isListEmpty(LinkedList* list)
{
    return (list &&
            (! (list -> head)) &&
            (! (list -> tail)) &&
            list -> length == 0) ? TRUE : FALSE;
}
