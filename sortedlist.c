/*
Code by: Or Yamin
Project: sorted list data structure
Date: 21/05/2024
Review by: Yan
Review date: 21/05/2024
Approved by: Yan
Approved date: 22/05/2024
*/



#include <stdlib.h> /* size_t malloc() free() */
#include <assert.h> /* assert() */

#include "dllist.h" 
#include "sortedlist.h"

struct sortedlist
{
    dllist_t *list;
    sortedlist_cmp_func_t compare;
};


sortedlist_t *SortedlistCreate(sortedlist_cmp_func_t compare)
{
	sortedlist_t *sorted_list = NULL;
	assert(NULL != compare);
	sorted_list = (sortedlist_t *)malloc(sizeof(sortedlist_t));
	if (NULL == sorted_list) 
	{
		return NULL;
	}
	
	sorted_list->list = DllistCreate();
	if (NULL == sorted_list->list) 
	{
		free(sorted_list);
		return NULL;
	}
	
	sorted_list->compare = compare;
	return sorted_list;
}



void SortedlistDestroy(sortedlist_t *sorted_list)
{
	assert(NULL != sorted_list);
	assert(NULL != sorted_list->list);
	DllistDestroy(sorted_list->list);
	free(sorted_list);
	sorted_list = NULL;
}



sortedlist_iter_t SortedlistNext(sortedlist_iter_t iter)
{
	sortedlist_iter_t next_iter = {NULL};
	assert(NULL != iter.iter);
	next_iter.iter = DllistNext(iter.iter);
	
	return next_iter;
}



sortedlist_iter_t SortedlistPrev(sortedlist_iter_t iter)
{
	sortedlist_iter_t prev_iter = {NULL};
	assert(NULL != iter.iter);
	prev_iter.iter = DllistPrev(iter.iter);
	
	
	return prev_iter;
}



sortedlist_iter_t SortedlistGetBegin(const sortedlist_t *sorted_list)
{
	sortedlist_iter_t sorted_iter = {NULL};
	assert(NULL != sorted_list);
	assert(NULL != sorted_list->list);
	sorted_iter.iter = DllistGetBegin(sorted_list->list);
	
	#ifndef NDEBUG
		sorted_iter.list = (sortedlist_t *)sorted_list;
	#endif /* NDEBUG */
	
	return sorted_iter;
}



sortedlist_iter_t SortedlistGetEnd(const sortedlist_t *sorted_list)
{
	sortedlist_iter_t sorted_iter = {NULL};
	assert(NULL != sorted_list);
	assert(NULL != sorted_list->list);
	sorted_iter.iter = DllistGetEnd(sorted_list -> list);
	
	#ifndef NDEBUG
		sorted_iter.list = (sortedlist_t *)sorted_list;
	#endif /* NDEBUG */ 
	
	return sorted_iter;
}



sortedlist_iter_t SortedlistInsert(sortedlist_t *sorted_list, void *data)
{
	sortedlist_iter_t sorted_iter = {NULL};
	assert(NULL != sorted_list);
	assert(NULL != data);
	assert(NULL != sorted_list->list);
	
	sorted_iter = SortedlistGetBegin(sorted_list);
	while(!SortedlistIsSameIter(sorted_iter, SortedlistGetEnd(sorted_list)) 
	&& 0 >= sorted_list->compare(SortedlistGetData(sorted_iter), data))
	{
		sorted_iter = SortedlistNext(sorted_iter);
	}
	sorted_iter.iter = DllistInsertBefore(sorted_list->list, data, 
															sorted_iter.iter);

	return sorted_iter;
}



sortedlist_iter_t SortedlistRemove(sortedlist_iter_t where)
{
	assert(NULL != where.iter);
	where.iter = DllistRemove(where.iter);
	return where;
}



size_t SortedlistSize(const sortedlist_t *sorted_list)
{
	assert(NULL != sorted_list);
	assert(NULL != sorted_list->list);
	return DllistSize(sorted_list->list);
}



void *SortedlistGetData(sortedlist_iter_t sorted_iter)
{
	assert(NULL != sorted_iter.iter);
	return DllistGetData(sorted_iter.iter);
}



sortedlist_iter_t SortedlistFind
	(sortedlist_t *sorted_list, 
	sortedlist_iter_t from, 
	sortedlist_iter_t to, 
	const void *param)
{	
	assert(NULL != sorted_list);
	assert(NULL != from.iter);
	assert(NULL != to.iter);
	assert(NULL != param);
	assert(NULL != from.list);
	assert(NULL != to.list);
	assert(NULL != sorted_list->list);
	assert(from.list == to.list);
	
	while(!SortedlistIsSameIter(from, to) && 
	sorted_list->compare(DllistGetData(from.iter), param) != 0 &&
	sorted_list->compare(DllistGetData(from.iter), param) < 0)
	{
		from = SortedlistNext(from);
	}
	return from;
}



sortedlist_iter_t SortedlistFindIf
	(sortedlist_iter_t from,
	sortedlist_iter_t to, 
	int (*is_match)(const void *data,const void *param), 
	void *param)
{
	assert(NULL != from.iter);
	assert(NULL != to.iter);
	assert(NULL != from.list);
	assert(NULL != to.list);
	assert(NULL != is_match);
	assert(NULL != param);
	assert(from.list == to.list);
	
	from.iter = DllistFind(param, from.iter, to.iter , is_match);
	return from;
}



int SortedlistIsSameIter(sortedlist_iter_t iter1, sortedlist_iter_t iter2)
{
	assert(NULL != iter1.iter);
	assert(NULL != iter2.iter);
	
	return DllistIsSameIter(iter1.iter, iter2.iter);
}



int SortedlistIsEmpty(const sortedlist_t *sorted_list)
{
	assert(NULL != sorted_list);
	assert(NULL != sorted_list->list);
	return DllistIsEmpty(sorted_list->list);
}



void *SortedlistPopFront(sortedlist_t *sorted_list)
{
	assert(NULL != sorted_list);
	assert(NULL != sorted_list->list);
	return DllistPopFront(sorted_list->list);
}



void *SortedlistPopBack(sortedlist_t *sorted_list)
{
	assert(NULL != sorted_list);
	assert(NULL != sorted_list->list);
	return DllistPopBack(sorted_list->list);
}



void SortedlistMerge(sortedlist_t *dest, sortedlist_t *src)
{
	sortedlist_iter_t dest_runner = {NULL};
	sortedlist_iter_t src_runner = {NULL};
	sortedlist_iter_t src_from = {NULL};
	
	assert(NULL != dest);
	assert(NULL != src);
	assert(NULL != dest->list);
	assert(NULL != src->list);
	assert(dest->compare == src->compare);
	
	dest_runner = SortedlistGetBegin(dest);
	src_runner = SortedlistGetBegin(src);
	src_from = src_runner;
	
	while(!SortedlistIsSameIter(dest_runner,SortedlistGetEnd(dest)))
	{
		if (!SortedlistIsSameIter(src_runner,SortedlistGetEnd(src)) && 0 > 
			dest->compare(SortedlistGetData(dest_runner), 
							SortedlistGetData(src_runner)))
		{
			dest_runner = SortedlistNext(dest_runner);
		}
		else
		{
			while(0 <= dest->compare(SortedlistGetData(dest_runner), 
					SortedlistGetData(src_runner)))
			{
				src_runner = SortedlistNext(src_runner);
			}
			DllistSplice(dest_runner.iter, src_from.iter, src_runner.iter);
			dest_runner = SortedlistNext(dest_runner);
			src_from = src_runner;
		}
	}
	
	if(!SortedlistIsEmpty(src))
	{
		DllistSplice(dest_runner.iter, src_from.iter, SortedlistGetEnd(src).iter);
	}
}


int SortedlistForEach
	(sortedlist_iter_t from, 
	sortedlist_iter_t to, 
	int (*action)(void *data, void *param), 
	void *param)
{
	assert(NULL != from.iter);
	assert(NULL != from.list);
	assert(NULL != to.iter);
	assert(NULL != to.list);
	assert(NULL != param);
	assert(NULL != action);
	assert(from.list == to.list);
	
	return DllistForEach(from.iter, to.iter, action, param);
}



