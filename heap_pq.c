/* 
   Code by: Or Yamin    
   Project: priority queue(heap) data structure
   Date: 
   Review by: 
   Review Date: 
   Approved by:
   Approval Date: 
*/

#include <stdlib.h> /* size_t malloc() free() */
#include <assert.h> /* assert() */

#include "heap.h"
#include "heap_pq.h"


struct heap_pq 
{
	heap_t *heap;
};


heap_pq_t *Heap_PQCreate(heap_pq_compare_func_t cmp_func) 
{
	heap_pq_t *heap_pqueue = NULL;
	
	assert(cmp_func != NULL);
	
	heap_pqueue = (heap_pq_t *)malloc(sizeof(heap_pq_t));
	
	if (heap_pqueue == NULL) 
	{
		return NULL;
	}
	
	heap_pqueue->heap = HeapCreate(cmp_func);
	if (heap_pqueue->heap == NULL) 
	{
		free(heap_pqueue);
		return NULL;
	}
	
	return heap_pqueue;
}



void Heap_PQDestroy(heap_pq_t *queue) 
{
	assert(queue != NULL);
    assert(queue->heap != NULL);
    
	HeapDestroy(queue->heap);
	free(queue);
}


int Heap_PQEnqueue(heap_pq_t *queue, void *data) 
{	
	assert(queue != NULL);
	assert(queue->heap != NULL);
	
	return HeapPush(queue->heap, data); 
}



void *Heap_PQDequeue(heap_pq_t *queue) 
{
	void *data = NULL;
	
    assert(queue != NULL);
    assert(queue->heap != NULL);
    
    data = HeapPeek(queue->heap);
    HeapPop(queue->heap);
    
    return data;
}




void *Heap_PQPeek(const heap_pq_t *queue) 
{
    assert(queue != NULL);
    assert(queue->heap != NULL);
    
    return HeapPeek(queue->heap);
}



int Heap_PQIsEmpty(const heap_pq_t *queue) 
{
	assert(queue != NULL);
	assert(queue->heap != NULL);
    
	return HeapIsEmpty(queue->heap);
}



size_t Heap_PQSize(const heap_pq_t *queue) 
{
	assert(queue != NULL);
	assert(queue->heap != NULL);
	
	return HeapSize(queue->heap);
}



void Heap_PQClear(heap_pq_t *queue) 
{
	assert(queue != NULL);
	assert(queue->heap != NULL);
	
	while (!HeapIsEmpty(queue->heap)) 
    {
		HeapPop(queue->heap);
    }
}



void *Heap_PQErase(heap_pq_t *queue, heap_pq_match_func_t is_match, void *param) 
{
	assert(queue != NULL);
	assert(is_match != NULL);
	assert(queue->heap != NULL);

	return HeapRemove(queue->heap, is_match, param);
}
