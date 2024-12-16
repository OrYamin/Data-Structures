/* 
   Code by: Or Yamin    
   Project: scheduler_heap heap data structure
   Date: 
   Review by: 
   Review Date: 
   Approved by: 
   Approval Date: 
*/


#include <stdlib.h> /* malloc() free() */
#include <assert.h> /* assert() */
#include <time.h> /* time() */
#include <unistd.h> /* sleep() */
#include <sys/types.h> /*size_t, time_t*/

#include "scheduler_heap.h"
#include "heap_pq.h"
#include "uid.h"
#include "task.h"



struct scheduler_heap
{
    heap_pq_t *heap_pq;
    int stop_flag;
};


static int TaskCompare(const void *task1, const void *task2) 
{
	return GetExecTime((const task_t *)task2) - GetExecTime((const task_t *)task1);
}


static int TaskUidMatch(const void *task, const void *uid) 
{
	return IsSameUID(GetUid((const task_t *)task), *(const ilrd_uid_t *)uid);
}


scheduler_heap_t *Scheduler_HeapCreate(void) 
{
	scheduler_heap_t *scheduler_heap = (scheduler_heap_t *)malloc(sizeof(scheduler_heap_t));
	if (NULL == scheduler_heap) 
	{
		return NULL;
    }

    scheduler_heap->heap_pq = Heap_PQCreate(TaskCompare);
    if (NULL == scheduler_heap->heap_pq) 
    {
        free(scheduler_heap);
        return NULL;
    }

    scheduler_heap->stop_flag = 0;
    return scheduler_heap;
}



void Scheduler_HeapDestroy(scheduler_heap_t *scheduler_heap) 
{
	assert(NULL != scheduler_heap);
	assert(NULL != scheduler_heap->heap_pq);

    Scheduler_HeapClear(scheduler_heap); 
    Heap_PQDestroy(scheduler_heap->heap_pq);
    free(scheduler_heap);
}


ilrd_uid_t Scheduler_HeapAdd(scheduler_heap_t *scheduler_heap, 
						time_t exec_time, 
						time_t interval_in_seconds, 
						int (*action)(void *params), 
						void *params) 
{
	
	task_t *task = NULL;
	
	assert(NULL != scheduler_heap);
	assert(NULL != action);
	assert(NULL != scheduler_heap->heap_pq);
	
	task = TaskCreate(exec_time, interval_in_seconds, action, params);
	if (NULL == task) 
	{
		return BadUID;
    }

	if (Heap_PQEnqueue(scheduler_heap->heap_pq, task) != 0) 
	{
        TaskDestroy(task);
		return BadUID;
	}

	return GetUid(task);
}


int Scheduler_HeapRemove(scheduler_heap_t *scheduler_heap, ilrd_uid_t uid) 
{
	task_t *task = NULL;
	
	assert(NULL != scheduler_heap);
	assert(NULL != scheduler_heap->heap_pq);
    
    task = (task_t *)Heap_PQErase(scheduler_heap->heap_pq, TaskUidMatch, &uid);
    if (NULL != task)
    {
        TaskDestroy(task);
        return SUCCESS;
    }
    return SCHEDULER_UID_NOT_FOUND;
}


int Scheduler_HeapRun(scheduler_heap_t *scheduler_heap) 
{
	task_t *task = NULL;
	time_t now = 0;
	unsigned int time_to_sleep = 0;
	
    assert(NULL != scheduler_heap);
	assert(NULL != scheduler_heap->heap_pq);

    scheduler_heap->stop_flag = 0;
    
    while (!Heap_PQIsEmpty(scheduler_heap->heap_pq) && 1 != scheduler_heap->stop_flag) 
    {
		task = Heap_PQPeek(scheduler_heap->heap_pq);
		now = time(NULL);
		Heap_PQDequeue(scheduler_heap->heap_pq);
		time_to_sleep = GetExecTime(task) - now;
		
		if (GetExecTime(task) < now) 
		{
			TaskDestroy(task);
		}
		else
		{	
			while(time_to_sleep)
			{
				time_to_sleep = sleep(time_to_sleep);
			}
			if (TaskRun(task) == 0)
            {
            	if (IntervalTime(task) > 0)
            	{
					UpdateExecTime(task, GetExecTime(task) + IntervalTime(task));
					Heap_PQEnqueue(scheduler_heap->heap_pq, task);
				}
				else
				{
					TaskDestroy(task);
				}
            }           
			else 
            {
            	TaskDestroy(task);
            	scheduler_heap->stop_flag = 1;
				return FAILURE;
            }
        } 
    }
   
	return (Heap_PQIsEmpty(scheduler_heap->heap_pq)) ? SCHEDULER_EMPTY : SCHEDULER_STOP;
}


void Scheduler_HeapStop(scheduler_heap_t *scheduler_heap) 
{
    assert(NULL != scheduler_heap);
	assert(NULL != scheduler_heap->heap_pq);

    scheduler_heap->stop_flag = 1;
}


size_t Scheduler_HeapSize(scheduler_heap_t *scheduler_heap) 
{
	assert(NULL != scheduler_heap);
	assert(NULL != scheduler_heap->heap_pq);

	return Heap_PQSize(scheduler_heap->heap_pq);
}

int Scheduler_HeapIsEmpty(scheduler_heap_t *scheduler_heap) 
{
	assert(NULL != scheduler_heap);
	assert(NULL != scheduler_heap->heap_pq);

	return Heap_PQIsEmpty(scheduler_heap->heap_pq);
}


void Scheduler_HeapClear(scheduler_heap_t *scheduler_heap) 
{
	assert(NULL != scheduler_heap);
	assert(NULL != scheduler_heap->heap_pq);

	while (!Heap_PQIsEmpty(scheduler_heap->heap_pq)) 
    {
		task_t *task = Heap_PQDequeue(scheduler_heap->heap_pq);
		TaskDestroy(task);
    }
}

