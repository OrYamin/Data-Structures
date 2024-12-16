/*approved by Yan Meiri on 13/05/2024
written by Or Yamin on 12/05/2024*/

#include <stddef.h> /*size_t*/
#include <assert.h> /*assert*/
#include <stdlib.h> /*malloc free*/

#include "sllist.h" /*sllist_t SllistCreate SllistDestroy SllistEnd SllistInsertBefore 
sllist_iter_t SllistBegin SllistCount SllistIsEmpty QAppend*/
#include "queue.h"


struct queue
{
	sllist_t *list;
};



queue_t *QCreate(void)
{
	queue_t *queue = (queue_t *)malloc(sizeof(struct queue));
	if(NULL == queue)
	{
		return NULL;
	}	
	
	queue->list = SllistCreate();
	if(NULL == queue->list)
	{
		free(queue);
		return NULL;
	}
	
	return queue;
}


void QDestroy(queue_t *queue)
{
	assert(NULL != queue);
	SllistDestroy(queue->list);
	queue->list = NULL;
	free(queue);
	queue = NULL;
}


int QEnqueue(queue_t *queue, void *data)
{
	sllist_iter_t dummy = NULL;
	sllist_iter_t status = NULL;
	assert(NULL != queue);
	dummy = SllistEnd(queue->list);
	status = SllistInsertBefore(queue->list, data, dummy);
	if(NULL == status)
	{
		return 1;
	}
	return 0;
}


void QDequeue(queue_t *queue)
{
	sllist_iter_t head = NULL;
	assert(NULL != queue);
	head = SllistBegin(queue->list);
	SllistRemove(head);
}


size_t QSize(const queue_t *queue)
{
	size_t size = 0;
	assert(NULL != queue);
	size = SllistCount(queue->list);
	return size;
}


int QIsEmpty(const queue_t *queue)
{
	assert(NULL != queue);
	return SllistIsEmpty(queue->list);
}


void *QPeek(const queue_t *queue)
{
	sllist_iter_t head = NULL;
	assert(NULL != queue);
	head = SllistBegin(queue->list);
	return SllistGetData(head);
}


void QAppend(queue_t *dest_queue, queue_t *src_queue)
{
	assert(NULL != dest_queue && NULL != src_queue);
	SllistAppend(dest_queue->list, src_queue->list);
}


