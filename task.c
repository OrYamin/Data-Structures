
#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */

#include "task.h"
#include "uid.h" 

struct task
{
    ilrd_uid_t uid;
    time_t exe_time;
    time_t interval_in_seconds;
    int (*action)(void *params);
    void *params;
};


task_t *TaskCreate(time_t exe_time, time_t interval_in_seconds, int (*action)(void *params), void *params) 
{
	task_t *task = NULL;
	
	assert(NULL != action);
	
	task = (task_t *)malloc(sizeof(task_t));
	
	if (NULL == task) 
	{
		return NULL;
    }

	task->uid = UIDCreate();
	if (IsSameUID(task->uid, BadUID)) 
    {
		free(task);
		return NULL;
    }

    task->exe_time = exe_time;
    task->interval_in_seconds = interval_in_seconds;
    task->action = action;
    task->params = params;

    return task;
}


void TaskDestroy(task_t *task) 
{
	assert(NULL != task);

	free(task);
}


int TaskRun(task_t *task) 
{
	assert(NULL != task);
	assert(NULL != task->action);

	return task->action(task->params);
}

ilrd_uid_t GetUid(const task_t *task) 
{
	assert(NULL != task);

	return task->uid;
}

time_t IntervalTime(const task_t *task)
{
	assert(NULL != task);

	return task->interval_in_seconds;
}


time_t GetExecTime(const task_t *task) 
{
	assert(NULL != task);

	return task->exe_time;
}

void UpdateExecTime(task_t *task, time_t new_exec_time) 
{
	assert(NULL != task);

	task->exe_time = new_exec_time;
}

