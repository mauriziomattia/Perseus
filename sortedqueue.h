/*
 *
 *   sortedqueue.h
 *
 *   Library of structures and functions to manage 
 *   sorted queues with respect to a given measure,
 *   which allows to get the minimal item and to
 *   insert and delete elements without any 
 *   restriction on the value of the item's key. 
 *   The queue will have always items with ordered 
 *   keys. 
 *   The library uses as data structure the balanced
 *   AVL trees.
 *
 *   Author: Maurizio Mattia
 *
 */



#ifndef __SORTEDQUEUE_H__
#define __SORTEDQUEUE_H__



/*----------------------*
 *  GLOBAL DEFINITIONS  *
 *----------------------*/

/*** The node of an AVL tree. ***/
typedef struct _avl_tree {
          int      Balance; /* It -1, 0 or +1 depending on the heigth of the two subtrees. */
   struct _avl_tree  *Left; /* Pointer to the Left subtree. */
   struct _avl_tree *Right; /* Pointer to the Right subtree. */
   struct _avl_tree    *Up; /* Pointer to the father node (Optional). */
          void       *Item; /* Pointer to the element associated to the node. */
} avl_tree;

/*** The header of a sorted queue. ***/
typedef struct sortedqueue_struct {
   avl_tree *ReadyNode; /* Pointer to the node in the tree corresponding to the element *
                         * of the queue to extract with "get" method or to observe with *
                         * "front" method. If it is NULL the queue is empty.            */
   int (*cmpFunc) (void *, void *); /* function used to define the metrics, returning -1, 0   *
                                     * or +1 depending if the first parameter is lower, equal *
                                     * or greater than the second parameter.                  */
   int      ElementNum; /* Number of items stored in the queue. */
   avl_tree      *Root; /* Pointer to the associated balanced (AVL) tree. */
} sorted_queue;



/*--------------------*
 *  GLOBAL FUNCTIONS  *
 *--------------------*/


/**
 *  Initializes the given sorted queue without 
 *  any elements, with a suited metric defined
 *  by the compare function.
 *  Before the use of a sorted queue this function
 *  has to be called.
 */

void initSortedQueue(sorted_queue *SQ,                 /* Ptr. to the queue to initialize. */
                     int (*cmpFunc) (void *, void *)); /* Compare function, defining the metric. */


/**
 *  Insert a new item in the queue finding the
 *  right position and keeping the tree balanced.
 */

void putSortedQueueItem(sorted_queue *SQ,  /* Ptr. to the receiving queue. */
                        void       *Item); /* Item to put in the queue. */


/**
 *  Gets the lower element in the queue and frees the memory allocated
 *  for the associated node.
 *  Returns the extracted Item.
 */
void *getSortedQueueItem(sorted_queue *SQ); // Ptr. to the sorted queue.


/**
 *  Returns the lower items in the queue, the ready one,
 *  without removing it from the queue. The lower with 
 *  respect to the metric defined by the cmpFunc.
 */

void *frontSortedQueueItem(sorted_queue *SQ); /* Ptr. to the tested queue. */


/**
 *  Return true if no items are stored in the queue.
 */

int isSortedQueueEmpty(sorted_queue *SQ); /* Ptr. to the tested queue. */


/**
 *  Returns the number of items stored in the queue.
 */
int sortedQueueSize(sorted_queue *SQ); /* Ptr. to the tested queue. */


/**
 *  Visit the queue iterating its items in ascending
 *  order and passing them to a call-back function.
 */

void visitSortedQueue(sorted_queue *SQ,          /* Queue to visit. */
                      void (*iterator) (int, avl_tree *, void *)); /* Process the item visited. */


#endif /* __SORTEDQUEUE_H__ */
