/* 
   Code by: Or Yamin    
   Project: scheduler data structure
   Date: 01/06/24
   Review by: Ido
   Review Date: 02/06/24
   Approved by: Ido
   Approval Date: 02/06/24
*/


#include <stdlib.h> /* malloc() free() */
#include <assert.h> /* assert() */
#include <time.h> /* time() */
#include <unistd.h> /* sleep() */
#include <sys/types.h> /*size_t, time_t*/

#include "pq.h"
#include "scheduler.h"
#include "uid.h"
#include "task.h"



struct scheduler
{
    pq_t *task_queue;
    int stop_flag;
};


static int TaskCompare(const void *task1, const void *task2) 
{
	return GetExecTime((const task_t *)task1) - GetExecTime((const task_t *)task2);
}


static int TaskUidMatch(const void *task, const void *uid) 
{
	return IsSameUID(GetUid((const task_t *)task), *(const ilrd_uid_t *)uid);
}


scheduler_t *SchedulerCreate(void) 
{
	scheduler_t *scheduler = (scheduler_t *)malloc(sizeof(scheduler_t));
	if (NULL == scheduler) 
	{
		return NULL;
    }

    scheduler->task_queue = PQCreate(TaskCompare);
    if (NULL == scheduler->task_queue) 
    {
        free(scheduler);
        return NULL;
    }

    scheduler->stop_flag = 0;
    return scheduler;
}



void SchedulerDestroy(scheduler_t *scheduler) 
{
	assert(NULL != scheduler);
	assert(NULL != scheduler->task_queue);

    SchedulerClear(scheduler); 
    PQDestroy(scheduler->task_queue);
    free(scheduler);
}


ilrd_uid_t SchedulerAdd(scheduler_t *scheduler, 
						time_t exec_time, 
						time_t interval_in_seconds, 
						int (*action)(void *params), 
						void *params) 
{
	
	task_t *task = NULL;
	
	assert(NULL != scheduler);
	assert(NULL != action);
	assert(NULL != scheduler->task_queue);
	
	task = TaskCreate(exec_time, interval_in_seconds, action, params);
	if (NULL == task) 
	{
		return BadUID;
    }

	if (PQEnqueue(scheduler->task_queue, task) != 0) 
	{
        TaskDestroy(task);
		return BadUID;
	}

	return GetUid(task);
}


int SchedulerRemove(scheduler_t *scheduler, ilrd_uid_t uid) 
{
	task_t *task = NULL;
	
	assert(NULL != scheduler);
	assert(NULL != scheduler->task_queue);
    
    task = (task_t *)PQErase(scheduler->task_queue, TaskUidMatch, &uid);
    if (NULL != task)
    {
        TaskDestroy(task);
        return SUCCESS;
    }
    return SCHEDULER_UID_NOT_FOUND;
}


int SchedulerRun(scheduler_t *scheduler) 
{
	task_t *task = NULL;
	time_t now = 0;
	unsigned int time_to_sleep = 0;
	
    assert(NULL != scheduler);
	assert(NULL != scheduler->task_queue);

    scheduler->stop_flag = 0;
    
    while (!PQIsEmpty(scheduler->task_queue) && 1 != scheduler->stop_flag) 
    {
		task = PQPeek(scheduler->task_queue);
		now = time(NULL);
		PQDequeue(scheduler->task_queue);
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
					PQEnqueue(scheduler->task_queue, task);
				}
				else
				{
					TaskDestroy(task);
				}
            }           
			else 
            {
            	TaskDestroy(task);
            	scheduler->stop_flag = 1;
				return FAILURE;
            }
        } 
    }
   
	return (PQIsEmpty(scheduler->task_queue)) ? SCHEDULER_EMPTY : SCHEDULER_STOP;
}


void SchedulerStop(scheduler_t *scheduler) 
{
    assert(NULL != scheduler);
	assert(NULL != scheduler->task_queue);

    scheduler->stop_flag = 1;
}


size_t SchedulerSize(scheduler_t *scheduler) 
{
	assert(NULL != scheduler);
	assert(NULL != scheduler->task_queue);

	return PQSize(scheduler->task_queue);
}

int SchedulerIsEmpty(scheduler_t *scheduler) 
{
	assert(NULL != scheduler);
	assert(NULL != scheduler->task_queue);

	return PQIsEmpty(scheduler->task_queue);
}


void SchedulerClear(scheduler_t *scheduler) 
{
	assert(NULL != scheduler);
	assert(NULL != scheduler->task_queue);

	while (!PQIsEmpty(scheduler->task_queue)) 
    {
		task_t *task = PQDequeue(scheduler->task_queue);
		TaskDestroy(task);
    }
}

