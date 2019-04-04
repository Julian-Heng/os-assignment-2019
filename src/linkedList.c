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
static void peek(LinkedListNode* node, void** voidPtr, int* isMalloc);
static void clearListRecurse(LinkedListNode** node);
static void freeNode(LinkedListNode** node);

/**
 * Allocates memory for a node
 **/
LinkedListNode* initNode(void* newValue, int isMalloc)
{
    LinkedListNode* newNode;

    /* Allocate memory in the heap for a linked list node */
    if ((newNode = (LinkedListNode*)malloc(sizeof(LinkedListNode))))
    {
        /* Set default values */
        newNode->next = NULL;
        newNode->prev = NULL;
        newNode->value = newValue;
        newNode->isMalloc = isMalloc;
    }

    return newNode;
}

/**
 * Allocates memory for a list
 **/
LinkedList* initList(void)
{
    LinkedList* newList;

    /* Allocate memory in the heap for a linked list */
    if ((newList = (LinkedList*)malloc(sizeof(LinkedList))))
    {
        /* Set default values */
        newList->head = NULL;
        newList->tail = NULL;
        newList->length = 0;
    }

    return newList;
}

/**
 * Inserts a node to the front of the list
 *
 * Contains a void pointer for the data and a boolean value indicating
 * if the data is malloced
 **/
void insertFirst(LinkedList* list, void* newValue, int isMalloc)
{
    LinkedListNode* newNode;

    /* Create a new node with newValue */
    newNode = initNode(newValue, isMalloc);

    /* Special case for empty list */
    if (list && isListEmpty(list))
    {
        list->head = newNode;
        list->tail = newNode;
    }
    else
    {
        /**
         * Set the previous pointer in the current head to newNode,
         * then set the next in newNode to current head.
         **/
        SET_PREV_TO_NODE(list->head, newNode)
        SET_NEXT_TO_NODE(newNode, list->head)
        list->head = newNode;
    }

    (list->length)++;
}

/**
 * Inserts a node to the end of the list
 *
 * Same parameters as insertFirst()
 **/
void insertLast(LinkedList* list, void* newValue, int isMalloc)
{
    LinkedListNode* newNode;

    /* Create a new node with newValue */
    newNode = initNode(newValue, isMalloc);

    /* Special case for empty list */
    if (list && isListEmpty(list))
    {
        list->head = newNode;
        list->tail = newNode;
    }
    else
    {
        /**
         * Set the previous pointer in newNode to point to the current tail,
         * then set the next in current tail to newNode. Then set newNode as
         * the new tail
         **/
        SET_PREV_TO_NODE(newNode, list->tail)
        SET_NEXT_TO_NODE(list->tail, newNode)
        list->tail = newNode;
    }

    (list->length)++;
}

/**
 * Remove the first node in the list
 *
 * We return a pointer to a node because we cannot free it within this
 * function. Thus the calling function will need to perform the freeing of the
 * node. If we free the node within this function, we lose access to the void
 * pointer within the node
 **/
LinkedListNode* removeFirst(LinkedList* list, void** voidPtr, int* isMalloc)
{
    LinkedListNode* node = list->head;

    /* If node is null, means that head is null and the list is empty */
    if (node)
    {
        peek(node, voidPtr, isMalloc);

        if (list->length == 1)
        {
            list->head = NULL;
            list->tail = NULL;
        }
        else
        {
            list->head = list->head->next;
        }

        (list->length)--;
    }
    else
    {
        *voidPtr = NULL;
        *isMalloc = -1;
    }

    return node;
}

/**
 * Same as removeFirst(), except we do it to the last node in the list
 **/
LinkedListNode* removeLast(LinkedList* list, void** voidPtr, int* isMalloc)
{
    LinkedListNode* node = list->tail;

    /* If node is null, means that tail is null and the list is empty */
    if (node)
    {
        peek(node, voidPtr, isMalloc);

        if (list->length == 1)
        {
            list->head = NULL;
            list->tail = NULL;
        }
        else
        {
            list->tail = list->tail->next;
        }

        (list->length)--;
    }
    else
    {
        *voidPtr = NULL;
        *isMalloc = -1;
    }

    return node;
}

/**
 * Getting the value in the first node without removing it from the list
 *
 * voidPtr is where the value is stored and isMalloc is a boolean that
 * determines if it is stored on the heap or stack
 **/
void peekFirst(LinkedList* list, void** voidPtr, int* isMalloc)
{
    peek(list->head, voidPtr, isMalloc);
}

/**
 * Same as peekFirst() except it peeks the end of the list
 **/
void peekLast(LinkedList* list, void** voidPtr, int* isMalloc)
{
    peek(list->tail, voidPtr, isMalloc);
}

static void peek(LinkedListNode* node, void** voidPtr, int* isMalloc)
{
    if (node)
    {
        *voidPtr = node->value;
        *isMalloc = node->isMalloc;
    }
    else
    {
        *voidPtr = NULL;
        *isMalloc = -1;
    }
}

int getListLength(LinkedList* list)
{
    return list->length;
}

/**
 * Removes list and list nodes
 **/
void clearList(LinkedList** list)
{
    if (*list)
    {
        if (! isListEmpty(*list))
        {
            clearListRecurse(&((*list)->head));
        }

        free(*list);
        *list = NULL;
    }
}

static void clearListRecurse(LinkedListNode** node)
{
    if (*node)
    {
        clearListRecurse(&((*node)->next));
        freeNode(node);
    }
}

/**
 * Wrapper function to free a node
 *
 * Required to check if the data a node holds is malloced or not
 **/
static void freeNode(LinkedListNode** node)
{
    if ((*node)->isMalloc && (*node)->value)
    {
        free((*node)->value);
        (*node)->value = NULL;
    }

    free(*node);
    *node = NULL;
}

int isListEmpty(LinkedList* list)
{
    return (list &&
            (! (list->head)) &&
            (! (list->tail)) &&
            list->length == 0) ? TRUE : FALSE;
}
