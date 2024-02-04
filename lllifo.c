/*******************************************************************************
 * Copyright (C) 2024 by <Student Name>
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. <Student Name> and the University of Colorado are not liable for
 * any misuse of this material.
 * ****************************************************************************/

/**
 * @file    lllifo.c
 * @brief
 *
 *
 * @author  <Student Name>
 * @date    <Date>
 *
 */

#include <stdlib.h>
#include "lllifo.h"


typedef struct Node Node;
struct Node {
    void* content;
    Node* next;
};
static const int nodeSize = sizeof(Node);

struct lllifo_s {
    Node* head;
    Node* unusedHead;
    Node* origPtr;
    int origCap;
    int len;
    int cap;
    int maxCap;
};
static const int lllifoSize = sizeof(lllifo_t);


// static void dumpll(lllifo_t *lifo) {
//     if(lifo == NULL) {
//         printf("lifo pointer is NULL\n");
//         return;
//     }

//     printf("LIFO DUMP:\n");
//     printf("\tlen: %d\n", lifo->len);
//     printf("\tcap: %d\n", lifo->cap);
//     printf("\tmax cap: %d\n", lifo->maxCap);
//     printf("\torig cap: %d\n", lifo->origCap);
//     printf("\torig ptr: %p\n", lifo->origPtr);

//     printf("\thead: ");
//     if(lifo->head == NULL) printf("(nil)");
//     else {
//         Node* nodePtr = lifo->head;
//         while(nodePtr != NULL) {
//             printf("(%s, %p) -> ", (char*)nodePtr->content, nodePtr->next);
//             nodePtr = nodePtr->next;
//         }
//     }
//     printf("\n");

//     printf("\tunused head: ");
//     if(lifo->unusedHead == NULL) printf("(nil)");
//     else {
//         Node* nodePtr = lifo->unusedHead;
//         while(nodePtr != NULL) {
//             printf("(%s, %p)", (char*)nodePtr->content, nodePtr->next);
//             nodePtr = nodePtr->next;
//         }
//     }
//     printf("\n");
// }

/*
 * Creates and initializes the LIFO
 *
 * Parameters:
 *   capacity  the initial size of the lifo, in number of elements. 
 *             Valid values are in the range of 0 to max_capacity(inclusive of both).
 *   max_capacity maximum capacity of the LIFO. Once the capacity equals
 *             max capacity, any further attempt to push will discard
 *             the oldest element and reuse that node for storing the
 *             newly pushd element. max_capacity > 0.
 * 
 * Returns: A pointer to a newly-created lllifo_t instance, or NULL in
 *   case of an error.
 */
lllifo_t *lllifo_create(int capacity, int max_capacity) {
    if(max_capacity <= 0 || capacity < 0 || capacity > max_capacity) return NULL;

    lllifo_t *lifo = (lllifo_t *) malloc(lllifoSize);
    if(lifo == NULL) return NULL;

    lifo->len = 0;
    lifo->cap = capacity;
    lifo->maxCap = max_capacity;
    lifo->origCap = capacity;
    lifo->origPtr = (Node*) malloc(capacity * nodeSize);

    // Init allocated mem as a ll of nodes
    for(int i = 0; i < capacity; i++) {
        Node* nodePtr = lifo->origPtr + i;
        Node* nextNodePtr = (i == capacity-1 ? NULL : (lifo->origPtr + i + 1));
        nodePtr->content = 0;
        nodePtr->next = nextNodePtr;
    }

    lifo->unusedHead = lifo->origPtr;
    lifo->head = NULL;

    return lifo;
}

/*
 * Adds("pushs") an element onto the LIFO, growing the LIFO by adding
 * additional elements, if necessary upto max_capacity. Thereafter,
 * it will recycle the oldest node to push the recent data.
 *
 * Parameters:
 *   lifo    The lifo in question
 *   element The element to push, which cannot be NULL
 * 
 * Returns:
 *   The new length of the LIFO on success, -1 on failure. It is an error to attempt 
 *   to push the NULL pointer. Also, giving a NULL for lifo is an error.
 */
int lllifo_push(lllifo_t *lifo, void *element) {
    if(lifo == NULL || element == NULL) return -1;

    // If we've reached max capacity, time to recycle the oldest node
    if(lifo->len == lifo->maxCap) {
        if(lifo->head == NULL) return -1;

        Node* oldestNode = lifo->head;
        Node* secondOldestNode = NULL;
        while(oldestNode->next != NULL) {
            secondOldestNode = oldestNode;
            oldestNode = oldestNode->next; // Traverse to end of list
        }

        oldestNode->content = element;
        if(secondOldestNode != NULL) { // If there's more than one node in the list
            secondOldestNode->next = NULL;
            oldestNode->next = lifo->head;
            lifo->head = oldestNode;
        }
    }
    // If we've reached current capacity, time to allocate a new node
    else if(lifo->len == lifo->cap) {
        Node* newNode = (Node*) malloc(nodeSize);
        if(newNode == NULL) { // If malloc fails
            lllifo_destroy(lifo);
            return -1;
        }

        newNode->content = element;
        newNode->next = lifo->head;
        lifo->head = newNode;

        lifo->cap++;
        lifo->len++;
    }
    // We still have capacity, grab a node from the unused list.
    else {
        Node* newNode = lifo->unusedHead;
        if(newNode == NULL) return -1;

        lifo->unusedHead = newNode->next;

        newNode->content = element;
        newNode->next = lifo->head;
        lifo->head = newNode;

        lifo->len++;
    }

    return lifo->len;
}

/*
 * Removes ("pops") an element from the LIFO, and returns it.
 *
 * Parameters:
 *   lifo  The lifo in question
 * 
 * Returns:
 *   The pop-ed element, or NULL if the LIFO was empty or if an error in parameters.
 */
void *lllifo_pop(lllifo_t *lifo) {
    if(lifo == NULL || lifo->head == NULL) return NULL;

    // Remove node from head ll
    Node* toPop = lifo->head;
    lifo->head = toPop->next;

    // Add node to ununsed ll
    toPop->next = lifo->unusedHead;
    lifo->unusedHead = toPop;

    lifo->len--;
    return toPop->content;
}

/*
 * Returns the number of elements currently in the LIFO. 
 *
 * Parameters:
 *   lifo  The lifo in question
 * 
 * Returns:
 *   The number of elements currently on the LIFO.
 */
int lllifo_length(lllifo_t *lifo) {
    if(lifo == NULL) return -1;
    return lifo->len;
}

/*
 * Returns the LIFO's current capacity
 *
 * Parameters:
 *   lifo  The lifo in question
 * 
 * Returns:
 *   The current capacity, in number of elements, for the LIFO.
 */
int lllifo_capacity(lllifo_t *lifo) {
    if(lifo == NULL) return -1;
    return lifo->cap;
}
/*
 * Returns the LIFO's max capacity
 *
 * Parameters:
 *   lifo  The lifo in question
 *
 * Returns:
 *   The max capacity, in number of elements, for the LIFO.
 */
int lllifo_max_capacity(lllifo_t *lifo) {
    if(lifo == NULL) return -1;
    return lifo->maxCap;
}

/*
 * Teardown function: Frees all dynamically allocated
 * memory. After calling this function, the lifo should not be used
 * again!
 *
 * Parameters:
 *   lifo  The lifo in question
 * 
 * Returns: none
 */
void lllifo_destroy(lllifo_t *lifo) {
    if(lifo == NULL) return;
    Node* origMallocEnd = lifo->origPtr + lifo->origCap;

    // Free all the nodes currently in use
    while(lifo->head != NULL) {
        Node* toFree = lifo->head;
        lifo->head = toFree->next;

        // If this node was a part of the original malloc, it will be freed later
        if(toFree >= lifo->origPtr && toFree < origMallocEnd) continue;
        free(toFree);
    }

    // Free all the allocated nodes not in use
    while(lifo->unusedHead != NULL) {
        Node* toFree = lifo->unusedHead;
        lifo->unusedHead = toFree->next;

        // If this node was a part of the original malloc, it will be freed later
        if(toFree >= lifo->origPtr && toFree < origMallocEnd) continue;
        free(toFree);
    }

    // Free the memory from the initial malloc and the lllifo header struct
    free(lifo->origPtr);
    free(lifo);
}

/*
 * Reset function: Resets the length of the LIFO to 0 and basically
 * reclaims all nodes for further use. Capacity and Max_Capacity 
 * stay intact but the LIFO is cleared. 
 *
 * Parameters:
 *   lifo  The lifo in question
 * 
 * Returns: none
 */
void lllifo_reset(lllifo_t *lifo) {
    if(lifo == NULL) return;
    while(lifo->head != NULL) lllifo_pop(lifo);
}
