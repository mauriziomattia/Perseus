/*
 *
 *   sortedqueue.c
 *
 *   Library of structures and functions to manage 
 *   sorted queues with respect to a given measure,
 *   which allows to get the minimal item and to
 *   insert and delete elements without any 
 *   restriction on the value of the item's key. 
 *   The queue will have always items with ordered 
 *   keys. The keys may be duplicated.
 *   The library uses as data structure the balanced
 *   AVL trees.
 *
 *   Author: Maurizio Mattia
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "sortedqueue.h"



/*-------------------*
 *  LOCAL FUNCTIONS  *
 *-------------------*/


/**
 *  Tool function used in this library to left rotate 
 *  an AVL tree given its root.
 *  Returns the new root of the tree.
 */

static avl_tree *avlLeftRotate(avl_tree *Root) 
{
   static avl_tree *RTree;
   
   RTree = Root->Right;
   Root->Right = RTree->Left;
   RTree->Left = Root;

   RTree->Up = Root->Up;
   Root->Up = RTree;

   Root->Balance--;
   if (RTree->Balance > 0) Root->Balance -= RTree->Balance;
   RTree->Balance--;
   if (Root->Balance < 0) RTree->Balance += Root->Balance;

   return RTree;
}


/**
 *  Tool function used in this library to right rotate 
 *  an AVL tree given its root.
 *  Returns the new root of the tree.
 */

static avl_tree *avlRightRotate(avl_tree *Root) 
{
   static avl_tree *LTree;
   
   LTree = Root->Left;
   Root->Left = LTree->Right;
   LTree->Right = Root;

   LTree->Up = Root->Up;
   Root->Up = LTree;

   Root->Balance++;
   if (LTree->Balance < 0) Root->Balance -= LTree->Balance;
   LTree->Balance++;
   if (Root->Balance > 0) LTree->Balance += Root->Balance;

   return LTree;
}


/**
 *  Add a new node to the balanced tree, keeping it
 *  balanced after the insertion.
 *  Returns a pointer to a tree node if a new node
 *  is created.
 */

static avl_tree *avlInsert(sorted_queue *SQ, // Link to the information about the queue.
                           avl_tree  *Root, // Root of the sub-tree to explore.
                           void      *Item) // Item to insert in the balanced tree.
{
   static avl_tree *NewNode;
   static int        Taller;

   /*** Inserts the item, being the subtree empty. ***/
   if (Root == NULL)
	   if ((NewNode = (avl_tree *)malloc(sizeof(avl_tree))) != NULL) {
         NewNode->Item = Item;
         NewNode->Balance = 0;
         NewNode->Left = NULL;
         NewNode->Right = NULL;
         NewNode->Up = NULL;
         SQ->ElementNum++;
         return NewNode;
      } else {
         fprintf(stderr, "ERROR (sortedqueue:avlInsert): Out of memory.");
         exit( EXIT_FAILURE );
      }

   /*** Looks for the right subtree where to insert the item. ***/
   else

      if ((*(SQ->cmpFunc))(Item, Root->Item) < 0) { // In the LEFT subtree
         if ((NewNode = avlInsert(SQ, Root->Left, Item)) != NULL) {
            if (Root->Left == NULL) { // There was a new node or a new balance?
               Taller = --(Root->Balance) < 0;
               NewNode->Up = Root;
               if (Root == SQ->ReadyNode) // Is the lower item?
                  SQ->ReadyNode = NewNode;
            } else
               Taller = 0;
            Root->Left = NewNode;
         } else
            if (Taller) {
               Root->Balance--;
               if (Root->Balance == -2) {
                  if (Root->Left->Balance == 1)
                     Root->Left = avlLeftRotate(Root->Left);
                  return avlRightRotate(Root);
               } else
                  Taller = Root->Balance == -1;
            }
         
      } else { // In the RIGHT subtree
         if ((NewNode = avlInsert(SQ, Root->Right, Item)) != NULL) {
            if (Root->Right == NULL) { // There was a new node or a new balance?
               Taller = ++(Root->Balance) > 0;
               NewNode->Up = Root;
            } else
               Taller = 0;
            Root->Right = NewNode;
         } else
            if (Taller) {
               Root->Balance++;
               if (Root->Balance == 2) {
                  if (Root->Right->Balance == -1)
                     Root->Right = avlRightRotate(Root->Right);
                  return avlLeftRotate(Root);
               } else
                  Taller = Root->Balance == 1;
            }
      }

   return NULL;
}



/**
 *  Extracts a Node from the balanced tree without to free 
 *  the corresponding memory space from the heap, keeping 
 *  the tree balanced.
 *  Returns the Root of the modified balanced tree.
 */

static avl_tree *avlExtractNode(sorted_queue *SQ, // Link to the information about the queue.
                                avl_tree   *Node) // The node to extract.
{
   static avl_tree *NextNode, *ScanNode;
   static void         *Item;
   static int         IsLeft;

   /*** Extracts Node from the tree. ***/
   if (Node->Left == NULL) 
      if (Node->Up == NULL) { /* Is Node the root? */
         ScanNode = Node->Right;
         if (ScanNode != NULL) {
            ScanNode->Up = NULL;
            SQ->ReadyNode = ScanNode;
         } else              /* Is the tree empty? */
            SQ->ReadyNode = NULL;
         return ScanNode;
      } else {
         ScanNode = Node->Up;
         if (ScanNode->Right == Node) {
            ScanNode->Right = Node->Right;
            ScanNode->Balance--;
         } else {
            ScanNode->Left = Node->Right;
            ScanNode->Balance++;
         }
         if (Node->Right != NULL) {
            Node->Right->Up = ScanNode;
            if (Node == SQ->ReadyNode)
               SQ->ReadyNode = Node->Right;
         } else
            if (Node == SQ->ReadyNode)
               SQ->ReadyNode = ScanNode;
      }

   else
      if (Node->Right == NULL)
         if (Node->Up == NULL) { /* Is Node the root? */
            ScanNode = Node->Left;
            if (ScanNode != NULL) 
               ScanNode->Up = NULL;
            return ScanNode;
         } else {
            ScanNode = Node->Up;
            if (ScanNode->Right == Node) {
               ScanNode->Right = Node->Left;
               ScanNode->Balance--;
            } else {
               ScanNode->Left = Node->Left;
               ScanNode->Balance++;
            }
            if (Node->Left != NULL)
               Node->Left->Up = ScanNode;
         }

      else { /* The node is a leaf. */
         NextNode = Node->Left;
	      while (NextNode->Right != NULL)
	         NextNode = NextNode->Right;
         if (NextNode->Up == Node) {
            Item = NextNode->Item;
            (*NextNode) = *Node;
            NextNode->Item = Item;
            NextNode->Balance++;
            ScanNode = NextNode;
         } else {
            ScanNode = NextNode->Up;
            ScanNode->Right = NextNode->Left;
            ScanNode->Balance--;
            Item = NextNode->Item;
            (*NextNode) = *Node;
            NextNode->Item = Item;
         }
      }

   /*** Re-balance the tree. ***/
   while (1) {

      if (ScanNode->Up != NULL)
         IsLeft = ScanNode->Up->Left == ScanNode;

      if (ScanNode->Balance == 2) {
         if (ScanNode->Right->Balance == -1)
            ScanNode->Right = avlRightRotate(ScanNode->Right);
         ScanNode = avlLeftRotate(ScanNode);
         if (ScanNode->Up != NULL)
            if (IsLeft)
               ScanNode->Up->Left = ScanNode;
            else
               ScanNode->Up->Right = ScanNode;

      } else       
         if (ScanNode->Balance == -2) {
            if (ScanNode->Right->Balance == 1)
               ScanNode->Left = avlLeftRotate(ScanNode->Left);
            ScanNode = avlRightRotate(ScanNode);
            if (ScanNode->Up != NULL)
               if (IsLeft)
                  ScanNode->Up->Left = ScanNode;
               else
                  ScanNode->Up->Right = ScanNode;
         }

      if (ScanNode->Up == NULL) // Is it the root?
         return ScanNode;

      if (ScanNode->Balance != 0)
         return SQ->Root;
      else {
         if (ScanNode->Up->Right == ScanNode)
            ScanNode->Up->Balance--;
         else
            ScanNode->Up->Balance++;
         ScanNode = ScanNode->Up;
      }
   }

   return NULL;
}



/**
 *  Visit in ascending order the node of the balanced
 *  tree. Items are passed to an "iterator" function.
 */

static void avlItemVisit(avl_tree  *Root,          /* Root of the sub-tree to explore. */
                         void (*iterator) (int, avl_tree *, void *)) /* Process the item visited. */
{
   static int Level;

   if (Root != NULL) {
      if (Root->Up == NULL) // Is it the root of the tree?
         Level = 0;
      else
         Level++;
      avlItemVisit(Root->Left, iterator);
      (*iterator)(Level, Root, Root->Item);
      avlItemVisit(Root->Right, iterator);
      Level--;
   }
}



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
                     int (*cmpFunc) (void *, void *)) /* Compare function, defining the metric. */
{
   SQ->cmpFunc = cmpFunc;
   SQ->ReadyNode = NULL;  // Empty queue.
   SQ->Root = NULL;
   SQ->ElementNum = 0;
}


/**
 *  Insert a new item in the queue finding the
 *  right position and keeping the tree balanced.
 */

void putSortedQueueItem(sorted_queue *SQ, /* Ptr. to the receiving queue. */
                        void *Item)      /* Item to put in the queue. */
{
   static avl_tree *NewRoot;

   if ((NewRoot = avlInsert(SQ, SQ->Root, Item)) != NULL) {
      if (SQ->Root == NULL)    // Is the only and then lower item?
         SQ->ReadyNode = NewRoot;
      SQ->Root = NewRoot;
   }
}


/**
 *  Gets the lower element in the queue and frees the memory allocated
 *  for the associated node.
 *  Returns the extracted Item.
 */
void *getSortedQueueItem(sorted_queue *SQ) // Ptr. to the sorted queue.
{
   static void        *Item;
   static avl_tree *DelNode;

   if (SQ->ElementNum == 0)
      return NULL;

   DelNode = SQ->ReadyNode;
   Item = DelNode->Item;
   SQ->Root = avlExtractNode(SQ, DelNode);
   SQ->ElementNum--;
   free(DelNode);

   return Item;
}



/**
 *  Returns the lower item in the queue, the ready one,
 *  without removing it from the queue. The lower with 
 *  respect to the metric defined by cmpFunc.
 */

void *frontSortedQueueItem(sorted_queue *SQ) /* Ptr. to the tested queue. */
{
   if (SQ->ReadyNode == NULL)
      return NULL;

   return (SQ->ReadyNode->Item);
}


/**
 *  Returns true if no items are stored in the queue.
 */

int isSortedQueueEmpty(sorted_queue *SQ) /* Ptr. to the tested queue. */
{
   return (SQ->Root == NULL);
}


/**
 *  Returns the number of items stored in the queue.
 */
int sortedQueueSize(sorted_queue *SQ) /* Ptr. to the tested queue. */
{
   return (SQ->ElementNum);
}



/**
 *  Visit the queue iterating its items in ascending
 *  order and passing them to a call-back function.
 */

void visitSortedQueue(sorted_queue *SQ,         /* Queue to visit. */
                      void (*iterator) (int, avl_tree *, void *)) /* Process the item visited. */
{
   avlItemVisit(SQ->Root, iterator);
}