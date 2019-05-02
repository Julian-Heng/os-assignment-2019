#ifndef LINKEDLIST_H
#define LINKEDLIST_H

/**
 * Struct Name: LinkedListNode
 * Purpose:     A node that is part of a linked list. Contains two
 *              pointer to another node, one for previous node and
 *              one for next node. By default, both will point to null.
 *              This node is node is designed to be doubly linked
 *
 *          +-----------+           +-----------+
 *          |           |   next    |           |
 *          |           | --------> |           |
 * Null <-- |   node    |           |   node    | --> Null
 *          |           | <-------- |           |
 *          |           |   prev    |           |
 *          +-----------+           +-----------+
 *
 **/

typedef struct LinkedListNode
{
    struct LinkedListNode* next;
    struct LinkedListNode* prev;
    void* value;
    int isMalloc;
} LinkedListNode;

/**
 * Struct Name: LinkedList
 * Purpose:     A "wrapper" struct for containing linked list nodes.
 *              It's essentially providing a pointer to the start and
 *              the end of a list. This list is designed to be double
 *              ended
 *
 *              +-----------+           +-----------+
 *     Null <-- |           |   next    |           | --> Null
 *              |           | --------> |           |
 * +------+     |   node    |           |   node    |     +------+
 * | head | --> |           | <-------- |           | <-- | tail |
 * +------+     |           |   prev    |           |     +------+
 *              +-----------+           +-----------+
 *
 **/

typedef struct LinkedList
{
    LinkedListNode* head;
    LinkedListNode* tail;
    int length;
} LinkedList;

/* Macros */
#define SET_NEXT_TO_NODE(a, b) \
    if ((a) && (b)) \
    { \
        (a)->next = (b); \
    }

#define SET_PREV_TO_NODE(a, b) \
    if ((a) && (b)) \
    { \
        (a)->prev = (b); \
    }

/* Function prototypes */
LinkedListNode* initNode(void* newValue, int malloc);
LinkedList* initList();
void insertFirst(LinkedList* list, void* newValue, int malloc);
void insertLast(LinkedList* list, void* newValue, int malloc);
LinkedListNode* removeFirst(LinkedList* list, void** voidPtr, int* isMalloc);
LinkedListNode* removeLast(LinkedList* list, void** voidPtr, int* isMalloc);
void peekFirst(LinkedList* list, void** voidPtr, int* isMalloc);
void peekLast(LinkedList* list, void** voidPtr, int* isMalloc);
int getListLength(LinkedList* list);
void clearList(LinkedList** list);
int isListEmpty(LinkedList* list);

#endif /* End of linkedList.h */
