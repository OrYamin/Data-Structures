/* 
   Code by: Or Yamin    
   Project: priority queue data structure
   Date: 29/05/24
   Review by: Amit
   Review Date: 29/05/24
   Approved by: Amit
   Approval Date: 29/05/24
*/

#include <stdlib.h> /* size_t malloc() free() */
#include <assert.h> /* assert() */

#include "sortedlist.h"
#include "pq.h"


struct pq 
{
	sortedlist_t *sorted_list;
};


pq_t *PQCreate(pq_compare_func_t cmp_func) 
{
	pq_t *pqueue = NULL;
	
	assert(cmp_func != NULL);
	
	pqueue = (pq_t *)malloc(sizeof(pq_t));
	
	if (pqueue == NULL) 
	{
		return NULL;
	}
	
	pqueue->sorted_list = SortedlistCreate(cmp_func);
	if (pqueue->sorted_list == NULL) 
	{
		free(pqueue);
		return NULL;
	}
	return pqueue;
}



void PQDestroy(pq_t *queue) 
{
	assert(queue != NULL);
	assert(queue->sorted_list != NULL);
    
	SortedlistDestroy(queue->sorted_list);
	free(queue);
}


int PQEnqueue(pq_t *queue, void *data) 
{	
	sortedlist_iter_t iter;
	
	assert(queue != NULL);
	assert(queue->sorted_list != NULL);
	
	iter = SortedlistInsert(queue->sorted_list, data);
	
	if (SortedlistIsSameIter(iter, SortedlistGetEnd(queue->sorted_list))) 
	{
		return 1; 
    }
	return 0; 
}



void *PQDequeue(pq_t *queue) 
{
    assert(queue != NULL);
    assert(queue->sorted_list != NULL);
    
    return SortedlistPopFront(queue->sorted_list);
}



void *PQPeek(const pq_t *queue) 
{
	sortedlist_iter_t iter;
	
    assert(queue != NULL);
    assert(queue->sorted_list != NULL);
    
    iter = SortedlistGetBegin(queue->sorted_list);
    return SortedlistGetData(iter);
}



int PQIsEmpty(const pq_t *queue) 
{
	assert(queue != NULL);
	assert(queue->sorted_list != NULL);
    
	return SortedlistIsEmpty(queue->sorted_list);
}



size_t PQSize(const pq_t *queue) 
{
	assert(queue != NULL);
	assert(queue->sorted_list != NULL);
	
	return SortedlistSize(queue->sorted_list);
}



void PQClear(pq_t *queue) 
{
	assert(queue != NULL);
	assert(queue->sorted_list != NULL);
	
	while (!PQIsEmpty(queue)) 
    {
		PQDequeue(queue);
    }
}



void *PQErase(pq_t *queue, pq_match_func_t is_match, void *param) 
{	
	sortedlist_iter_t from;
	sortedlist_iter_t to;
	sortedlist_iter_t found;
	void *data = NULL;
	
	assert(queue != NULL);
	assert(is_match != NULL);
	assert(queue->sorted_list != NULL);

	from = SortedlistGetBegin(queue->sorted_list);
	to = SortedlistGetEnd(queue->sorted_list);
	found = SortedlistFindIf(from, to, is_match, param);
	
	if (SortedlistIsSameIter(found, to)) 
	{
        return NULL;
    }
    
	data = SortedlistGetData(found);
	SortedlistRemove(found);
	return data;
}



