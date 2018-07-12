/**
 * File: clist.h
 * -------------
 * Generic, doubly linked list data structure interface.
 */

#ifndef _CLIST_H_INCLUDED
#define _CLIST_H_INCLUDED

#include "stdlib.h"

typedef void (*CleanupElemFn)(void *addr);

typedef struct CListImplementation CList;
typedef void* clist_iterator;
typedef const void* clist_const_iterator;

CList* clist_create(size_t elemsz, CleanupElemFn fn);
void clist_dispose(CList* cl);
void clist_clear(CList* cl);
int clist_count(const CList* cl);

void* clist_front(const CList* cl);
void* clist_back(const CList* cl);

void clist_push_front(CList* cl, const void* source);
void clist_push_back(CList* cl, const void* source);
void clist_insert(CList* cl, const void* source, int i);
void clist_erase(CList* cl, int i);

void clist_pop_front(CList* cl);
void clist_pop_back(CList* cl);

clist_const_iterator clist_begin(const CList* cl);
clist_const_iterator clist_end();
clist_const_iterator clist_next(clist_const_iterator it);
clist_const_iterator clist_prev(clist_const_iterator it);
void clist_remove(CList* cl, clist_iterator it);
void *clist_data(clist_const_iterator it);

#endif // _CLIST_H_INCLUDED
