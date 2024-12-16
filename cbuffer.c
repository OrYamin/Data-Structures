/*
Code by: Or Yamin
Project: Circular buffer data structure
Date: 15/05/2024
Review by: Amit
Review date: 15/05/2024
Approved by: Amit
Approved date: 1g/05/2024
*/


#include <sys/types.h> /*size_t ssize_t*/
#include <stdlib.h> /*malloc free*/
#include <string.h> /* memcpy */
#include <assert.h> /*assert*/

#include "cbuffer.h"

#define OFFSET(st, m) ((size_t)&(((st *)0)->m))

struct cbuffer
{
    ssize_t write_offset;
    ssize_t read_offset;
    size_t capacity;
    char data[1];
};



cbuffer_t *CBuffCreate(size_t capacity)
{
	cbuffer_t *buffer = (cbuffer_t *)malloc(OFFSET(cbuffer_t, data) + sizeof(char)*capacity);
	
	if(NULL == buffer)
	{	
		return NULL;
	}
	
	buffer->write_offset = -1;
	buffer->read_offset = -1;
	buffer->capacity = capacity;
	
	return buffer;
}


void CBuffDestroy(cbuffer_t *buffer)
{
	assert(NULL != buffer);
	
	free(buffer);
}
	

size_t CBuffFreeSpace(const cbuffer_t *buffer)
{
	assert(NULL != buffer);
	
	if(buffer->write_offset == buffer->read_offset && 0 == CBuffIsEmpty(buffer))
	{
		return 0;
	}
	
	if(buffer->write_offset >= buffer->read_offset)
	{
		return (((buffer->capacity))-(buffer->write_offset - buffer->read_offset));
	}
	else
	{
		return (buffer->read_offset - buffer->write_offset);
	}
}


int CBuffIsEmpty(const cbuffer_t *buffer)
{
	assert(NULL != buffer);
	
	return (buffer->write_offset == -1) ? 1 : 0;
}



size_t CBuffSize(const cbuffer_t *buffer)
{
	assert(NULL != buffer);
	
	return buffer->capacity;
}


ssize_t CBuffRead(cbuffer_t *buffer, void *dest, size_t count)
{
    ssize_t counter = 0;
    size_t data_available = 0;
    char *dest_char = NULL;
	size_t first_part = 0;
	
    assert(NULL != buffer);
    assert(NULL != dest);
    
    data_available = buffer->capacity - CBuffFreeSpace(buffer);
    first_part = buffer->capacity - buffer->read_offset;
	dest_char = (char *)dest;
	
    if (CBuffIsEmpty(buffer))
    {
        return counter;
    }

    if (count > data_available)
    {
        count = data_available;
    }

    
    if (first_part >= count)
    {
        memcpy(dest_char, buffer->data + buffer->read_offset, count);
        buffer->read_offset = (buffer->read_offset + count) % buffer->capacity;
    }
    else
    {
        memcpy(dest_char, buffer->data + buffer->read_offset, first_part);
        memcpy(dest_char + first_part, buffer->data, count - first_part);
        buffer->read_offset = count - first_part;
    }

    counter = count;

    if (buffer->read_offset == buffer->write_offset)
    {
        buffer->read_offset = -1;
        buffer->write_offset = -1;
    }

    return counter;
}



ssize_t CBuffWrite(cbuffer_t *buffer, const void *src, size_t count)
{
    ssize_t counter = 0;
    size_t space_left = 0;
    const char *src_char = NULL;
	size_t first_part = 0;
	
    assert(NULL != buffer);
    assert(NULL != src);

	counter = count;
	space_left = CBuffFreeSpace(buffer);
	first_part = buffer->capacity - buffer->write_offset;
	src_char = (const char *)src;
	
    if (-1 == buffer->write_offset)
    {
        buffer->read_offset = 0;
        buffer->write_offset = 0;
    }
    if(count > buffer->capacity)
    {
    	count %= buffer->capacity;
    	memcpy(buffer->data+buffer->write_offset, src_char + count, 
														first_part);
		memcpy(buffer->data, src_char + count + first_part,
												buffer->capacity - first_part);
	}
	
    else if (first_part >= count)
    {
        memcpy(buffer->data + buffer->write_offset, src_char, count);
        buffer->write_offset = (buffer->write_offset + count) % buffer->capacity;
    }
    else
    {
        memcpy(buffer->data + buffer->write_offset, src_char, first_part);
        memcpy(buffer->data, src_char + first_part, count - first_part);
        buffer->write_offset = count - first_part;
    }


    if (count > space_left)
    {
        buffer->read_offset = buffer->write_offset;
    }

    return counter;
}
	
	

