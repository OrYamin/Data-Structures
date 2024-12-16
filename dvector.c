/*approved by Amit
written by Or Yamin on 06/05/2024*/

#include <stdlib.h> /* malloc, realloc, free */
#include <assert.h> /* assert */

#include "dvector.h"

#define GROWTH_FACTOR 2

struct dvector 
{
	size_t size;
	size_t capacity;
	size_t element_size;
	char *buffer;
};


static int IsDvectorFull(size_t capacity, size_t size);


dvector_t *DvectorCreate(size_t capacity, size_t element_size) 
{
	dvector_t *vector = (dvector_t *)malloc(4 * sizeof(size_t));
	assert(0 < capacity);
	assert(0 < element_size);

	if (NULL == vector) 
	{
		return NULL;
	}

	vector->size = 0;
	vector->capacity = capacity;
	vector->element_size = element_size;
	vector->buffer = (char *)malloc(capacity * element_size * sizeof(char));
	if (NULL == vector->buffer) 
    {
        free(vector);
        return NULL;
    }
	return vector;
}

void DvectorDestroy(dvector_t *vector) 
{
	assert( NULL != vector);
	free(vector->buffer);
	free(vector);
}

int DvectorPushBack(dvector_t *vector, const void *new_element) 
{
	char *temp_p = NULL;
	size_t size_element = vector->size * vector->element_size;
	size_t i = 0;
	
	assert(NULL != vector);
	assert(NULL != new_element);
	
	temp_p = (char *)new_element;

	if (IsDvectorFull(vector->size, vector->capacity))
	{
		DvectorReserve(vector, vector->size * GROWTH_FACTOR);
	}
	
	for( i = 0; i < vector->element_size; i++ , temp_p++)
	{
		*(vector->buffer + size_element + i) = *temp_p;
	}
	++vector->size;

	return 0;
}

void DvectorPopBack(dvector_t *vector) 
{
	assert( NULL != vector);
	assert(vector->size > 0);
	--vector->size;
}

int DvectorShrink(dvector_t *vector) 
{
	assert(NULL != vector);
	return DvectorReserve(vector, vector->size);
}

int DvectorReserve(dvector_t *vector, size_t new_size) 
{
	char *new_buffer;
	assert(NULL != vector);
	
	new_buffer = (char *)realloc(vector->buffer,
												new_size*vector->element_size);
	if (!new_buffer) 
	{
		return -1;
	}
	
	
	if(new_size < vector->size)
	{
		vector->size = new_size;
	}
	
	vector->buffer = new_buffer;
	vector->capacity = new_size;

	return 0;
}

size_t DvectorSize(const dvector_t *vector) 
{
	assert(NULL != vector);
	return vector->size;
}

size_t DvectorCapacity(const dvector_t *vector) 
{
	assert(NULL != vector);
	return vector->capacity;
}

void *DvectorGetElement(const dvector_t *vector, size_t index) 
{
	assert(NULL != vector);
	assert(index < vector->size );
	return vector->buffer + index * vector->element_size;
}


static int IsDvectorFull(size_t size, size_t capacity)
{
	return (capacity == size);
}
