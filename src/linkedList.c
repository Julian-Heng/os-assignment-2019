/**
 * Filename: linkedList.c
 * Author:   Juilan Heng (19473701)
 * Purpose:  Provide the initialisation of a doubly linked double ended
 *           linked list as well as several utility functions.
 *
 * Note:     Modified linked list from UCP assignment 2018
 **/

#include <stdlib.h>
#include "linkedList.h"

#define FALSE 0
#define TRUE !FALSE

/* Static functions */
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
         * Set the previous pointer in newNode to point to the current
         * tail, then set the next in current tail to newNode. Then set
         * newNode as the new tail
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
 * function. Thus the calling function will need to perform the freeing
 * of the node. If we free the node within this function, we lose access
 * to the void pointer within the node
 **/
LinkedListNode* removeFirst(LinkedList* list, void** voidPtr, int* isMalloc)
{
    LinkedListNode* node = list->head;

    *voidPtr = NULL;
    *isMalloc = -1;

    /* If node is null, means that head is null and the list is empty */
    if (node)
    {
        peek(node, voidPtr, isMalloc);

        /* Special case for single node list */
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

    return node;
}

/**
 * Same as removeFirst(), except we do it to the last node in the list
 **/
LinkedListNode* removeLast(LinkedList* list, void** voidPtr, int* isMalloc)
{
    LinkedListNode* node = list->tail;

    *voidPtr = NULL;
    *isMalloc = -1;

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

    return node;
}

/**
 * Sets the value of the selected node to the void pointer
 **/
void peek(LinkedListNode* node, void** voidPtr, int* isMalloc)
{
    *voidPtr = NULL;
    *isMalloc = -1;

    /* Set to voidPtr to null if node is null */
    if (node)
    {
        *voidPtr = node->value;
        *isMalloc = node->isMalloc;
    }
}

/**
 * Removes list and list nodes
 **/
void clearList(LinkedList** list)
{
    LinkedListNode* node;

    if (*list)
    {
        while ((*list)->head != NULL)
        {
            node = (*list)->head;
            (*list)->head = (*list)->head->next;
            freeNode(&node);
            node = NULL;
        }

        free(*list);
        *list = NULL;
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
