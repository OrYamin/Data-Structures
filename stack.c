/*Approved by Yan
was written by or yamin on 02/05/2024*/

#include <stddef.h> /* size_t */
#include <stdio.h> /* printf */
#include <stdlib.h> /* free, malloc */
#include <assert.h> /* assert */

#include "stack.h"


struct stack
{
    size_t capacity;
    size_t size;
    size_t element_size;
    char *buffer;
};


stack_t *StackCreate(size_t capacity, size_t element_size)
{
	size_t buffer_size = element_size*capacity;
	stack_t *stack_ptr = (stack_t *)malloc(sizeof(stack_t) + (buffer_size * sizeof(char)));
	
	if( NULL == stack_ptr )
	{
		free(stack_ptr);
		return NULL;
	}
	
	stack_ptr->size = 0;
	stack_ptr->element_size = element_size;
	stack_ptr->capacity = capacity;
	stack_ptr->buffer = ((char *)stack_ptr + (sizeof(stack_t)));
	return stack_ptr;
}


void StackDestroy(stack_t *stack)
{
	free(stack);
}


void StackPush(stack_t *stack, void *new_element)
{
	char *temp_p = (char *)new_element;
	size_t size_element = stack->size * stack->element_size;
	size_t i = 0;
	
	if( stack->size == stack->capacity)
	{
		fprintf(stderr, "Stack is full, cannot push\n");
		return;
	}
	
	for( i = 0; i < stack->element_size; i++ , temp_p++)
	{
		*(stack->buffer + size_element + i) = *temp_p;

	}
	stack->size++;
}
		
	
void StackPop(stack_t *stack)
{
	assert(stack);
	assert(stack->size);

	stack->size = stack->size - 1;
}	


int StackIsEmpty(const stack_t *stack)
{
	return stack->size == 0 ? 1 : 0;
}


size_t StackGetSize(const stack_t *stack)
{
	return stack->size;
}


size_t StackGetCapacity(const stack_t *stack)
{
	return stack->capacity;
}


void *StackPeek(const stack_t *stack)
{
	size_t top_index = 0;
	
	if (StackIsEmpty(stack)) 
	{
		fprintf(stderr, "Stack is empty, cannot peek\n");
		return NULL;
	}
    
	top_index = (stack->size - 1) * stack->element_size;
	return &(stack->buffer[top_index]);
}

