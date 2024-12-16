#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "fsq.h"

struct fsq {
	void **buffer;
	size_t size;
	size_t read_index;
	size_t write_index;
	pthread_mutex_t enqueue_mutex;
	pthread_mutex_t dequeue_mutex;
	sem_t empty;
	sem_t full;
};


fsq_t *FSQCreate(size_t size) 
{
	fsq_t *fsq = (fsq_t *)malloc(sizeof(fsq_t));
	if (fsq == NULL) 
	{
		return NULL;
	}
	
	fsq->buffer = (void **)malloc(sizeof(void *) * size);
	if (fsq->buffer == NULL) 
	{
		free(fsq);
		return NULL;
	}
	
	fsq->size = size;
	fsq->read_index = 0;
	fsq->write_index = 0;
	pthread_mutex_init(&fsq->enqueue_mutex, NULL);
	pthread_mutex_init(&fsq->dequeue_mutex, NULL);
	sem_init(&fsq->empty, 0, size);
	sem_init(&fsq->full, 0, 0);
	
	return fsq;
}

void FSQDestroy(fsq_t *fsq) 
{
	pthread_mutex_destroy(&fsq->enqueue_mutex);
	pthread_mutex_destroy(&fsq->dequeue_mutex);
	sem_destroy(&fsq->empty);
	sem_destroy(&fsq->full);
	free(fsq->buffer);
	free(fsq);
}

void FSQEnqueue(fsq_t *fsq, void *item) 
{
	sem_wait(&fsq->empty);
	pthread_mutex_lock(&fsq->enqueue_mutex);
	fsq->buffer[fsq->write_index] = item;
	fsq->write_index = (fsq->write_index + 1) % fsq->size;
	pthread_mutex_unlock(&fsq->enqueue_mutex);
	sem_post(&fsq->full);
}

void *FSQDequeue(fsq_t *fsq) 
{
	void *item = NULL;
	
	sem_wait(&fsq->full);
	pthread_mutex_lock(&fsq->dequeue_mutex);
	item = fsq->buffer[fsq->read_index];
	fsq->read_index = (fsq->read_index + 1) % fsq->size;
	pthread_mutex_unlock(&fsq->dequeue_mutex);
	sem_post(&fsq->empty);
	
	return item;
}

