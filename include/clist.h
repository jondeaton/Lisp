/**
 * File: clist.h
 * -------------
 * Generic, doubly linked list data structure interface.
 */

#ifndef _CLIST_H_INCLUDED
#define _CLIST_H_INCLUDED

#include "stdlib.h"

typedef struct CListImplementation CList;
typedef void (*CleanupElemFn)(void *element);

/**
 * Function: clist_create
 * ----------------------
 * Constructs a linked list
 * @param elem_size The size of each element in the linked list
 * @param cleanupFn Cleanup function
 * @return Pointer to linked list data structure
 */
CList* clist_create(size_t elem_size, CleanupElemFn cleanupFn);

/**
 * Function: clist_dispose
 * -----------------------
 * Dispose of the linked list
 * @param cl The linked list to dispose of
 */
void clist_dispose(CList* cl);

/**
 * Function: clist_clear
 * ---------------------
 * Removes all elements from the list
 * @param cl The clist to remove ements from
 */
void clist_clear(CList* cl);

/**
 * Function: clist_count
 * ---------------------
 * Count the number of elements in the list
 * @param cl The clist to get the size of
 * @return The number of elements in the linked list
 */
int clist_count(const CList* cl);

/**
 * Function: clist_front
 * ---------------------
 * Gets a pointer to the front of the list
 * @param cl The list to get the first element of
 * @return Pointer to the data at the front of the list
 */
void* clist_front(const CList* cl);

/**
 * Function: clist_back
 * --------------------
 * Get the last element in the clist
 * @param cl The list to get the last element of
 * @return Pointer to the data at the back of the list
 */
void* clist_back(const CList* cl);

/**
 * Function: clist_next
 * --------------------
 * Get the next element of the list
 * @param data Pointer to the current element
 * @return Pointer to the next element of the list
 * or NULL if data is the last element.
 */
void *clist_next(const void *data);

/**
 * Function: clist_prev
 * --------------------
 * Get the previous element of the list
 * @param data Pointer to the current element
 * @return Pointer to the previous element of the list or
 * NULL if data is the first element of the list
 */
void *clist_prev(const void *data);

/**
 * Function: clist_push_front
 * --------------------------
 * Add an element to the front of a list
 * @param cl The list to push an element to
 * @param data Pointer to the data to insert at the front of the list
 */
void clist_push_front(CList* cl, const void* data);

/**
 * Function: clist_push_back
 * -------------------------
 * @param cl The list to append an element to
 * @param data Pointer to the data to put at the back of the list
 */
void clist_push_back(CList* cl, const void* data);

/**
 * Function: clist_insert
 * ----------------------
 * Insert an element into a list at a specified index.
 * @note Time complexity: O(index)
 * @param cl List to insert an element into
 * @param data
 * @param index The index (zero indexed) to insert the element at
 */
void clist_insert(CList* cl, const void* data, int index);

/**
 * Function: clist_erase
 * ---------------------
 * Erase the data at a specific index
 * @param cl The list to erase the element from
 * @param index The index of the element to erase
 */
void clist_erase(CList* cl, int index);

/**
 * Function: clist_remove
 * ----------------------
 * Remove an element from
 * @param cl The list to remove the element from
 * @param data Pointer to the data entry in the list to remove
 */
void clist_remove(CList *cl, void *data);

/**
 * Function: clist_pop_front
 * -------------------------
 * Removes the first element from the list
 * @param cl The list to remvoe the first element from
 */
void clist_pop_front(CList* cl);

/**
 * Function: clist_pop_back
 * ------------------------
 * Removes the last element in the list
 * @param cl The list to remove the last element from
 */
void clist_pop_back(CList* cl);

#endif // _CLIST_H_INCLUDED
