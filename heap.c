/*
Code by: Or Yamin
Project: Heap data structure
Date: 30/06/2024
Review by: Amit
Review date: 01/07/2024
Approved by: Amit
Approved date: 01/07/2024
*/

#include <assert.h> /* assert */
#include <stdlib.h> /* size_t malloc free */

#include <stdio.h>

#include "heap.h"
#include "dvector.h"

#define ADDRESS_SIZE sizeof(size_t)
#define INIT_VECTOR_SIZE 5

struct heap
{
	dvector_t *vector;
	heap_compare_func_t cmp_func;
};

static void HeapifyUp(heap_t *heap, size_t index);
static void HeapifyDown(heap_t *heap, size_t index);
static void SwapElements(dvector_t *vector, size_t index1, size_t index2);
static size_t GetParentIndex(size_t index);
static size_t GetLeftChildIndex(size_t index);
static size_t GetRightChildIndex(size_t index);
static size_t FindIndexToSwap(heap_t *heap, size_t index);
static dvector_t *GetVector(heap_t *heap);

heap_t *HeapCreate(heap_compare_func_t compare_func)
{
	heap_t *heap = NULL;

	assert(NULL != compare_func);

	heap = (heap_t *)malloc(sizeof(heap_t));
	if (NULL == heap)
	{
		return NULL;
	}

	heap->vector = DvectorCreate(INIT_VECTOR_SIZE, ADDRESS_SIZE);
	if (NULL == heap->vector)
	{
		free(heap);
		return NULL;
	}

	heap->cmp_func = compare_func;

	return heap;
}

void HeapDestroy(heap_t *heap)
{
	assert(NULL != heap);

	DvectorDestroy(GetVector(heap));
	free(heap);
}

int HeapPush(heap_t *heap, void *data)
{
	assert(NULL != heap);
	assert(NULL != heap->vector);

	if (0 != DvectorPushBack(GetVector(heap), &data))
	{
		return 1;
	}

	HeapifyUp(heap, DvectorSize(GetVector(heap)) - 1);

	return 0;
}

void HeapPop(heap_t *heap)
{
	size_t last_index = HeapSize(heap) - 1;

	assert(NULL != heap);
	assert(NULL != heap->vector);

	SwapElements(GetVector(heap), 0, last_index);
	DvectorPopBack(GetVector(heap));

	HeapifyDown(heap, 0);
}

void *HeapPeek(const heap_t *heap)
{
	assert(NULL != heap);
	assert(NULL != heap->vector);

	return *(void **)DvectorGetElement(heap->vector, 0);
}

size_t HeapSize(const heap_t *heap)
{
	assert(NULL != heap);
	assert(NULL != heap->vector);

	return DvectorSize(heap->vector);
}

int HeapIsEmpty(const heap_t *heap)
{
	assert(NULL != heap);
	assert(NULL != heap->vector);

	return (0 == HeapSize(heap));
}

void *HeapRemove(heap_t *heap, heap_match_func_t is_match, void *data_to_match)
{
	size_t n = HeapSize(heap);
	size_t i = 0;
	size_t last_index = n - 1;
	void *data = NULL;

	assert(NULL != heap);
	assert(NULL != heap->vector);
	assert(NULL != is_match);

	for (; i < n; ++i)
	{
		data = *(void **)DvectorGetElement(GetVector(heap), i);
		if (1 == is_match(data, data_to_match))
		{
			SwapElements(GetVector(heap), last_index, i);
			DvectorPopBack(GetVector(heap));
			HeapifyDown(heap, i);

			return data;
		}
	}

	return NULL;
}


/**************************************** Helpers *****************************/
static void HeapifyUp(heap_t *heap, size_t index)
{
	size_t parent_index = GetParentIndex(index);

	while (index > 0 && 
			heap->cmp_func(*(void **)DvectorGetElement(GetVector(heap), index), 
				*(void **)DvectorGetElement(GetVector(heap), parent_index)) > 0)
	{
		SwapElements(GetVector(heap), index, parent_index);
		index = parent_index;
		parent_index = GetParentIndex(index);
	}
}

static void HeapifyDown(heap_t *heap, size_t index)
{
	size_t index_to_swap = 0;

	while (index < HeapSize(heap) && index != (index_to_swap = FindIndexToSwap(heap, index)))
	{
		SwapElements(GetVector(heap), index, index_to_swap);
		index = index_to_swap;
	}
}

static void SwapElements(dvector_t *vector, size_t index1, size_t index2)
{
	void **first_data = (void **)DvectorGetElement(vector, index1);
	void **second_data = (void **)DvectorGetElement(vector, index2);
	
	void *temp = *first_data;
	*first_data = *second_data;
	*second_data = temp;
}

static size_t GetParentIndex(size_t index)
{
	return (index - 1) / 2;
}

static size_t GetLeftChildIndex(size_t index)
{
	return 2 * index + 1;
}

static size_t GetRightChildIndex(size_t index)
{
	return 2 * index + 2;
}

static size_t FindIndexToSwap(heap_t *heap, size_t index)
{
	size_t left_child_index = GetLeftChildIndex(index);
	size_t right_child_index = GetRightChildIndex(index);
	size_t index_to_swap = index;
	
	if (left_child_index < HeapSize(heap) &&
		heap->cmp_func(*(void **)DvectorGetElement(GetVector(heap), 
															  left_child_index),
					   *(void **)DvectorGetElement(GetVector(heap), index)) > 0)
	{
		index_to_swap = left_child_index;
	}

	if (right_child_index < HeapSize(heap) &&
		heap->cmp_func(*(void **)DvectorGetElement(GetVector(heap), 
															 right_child_index),
			   *(void **)DvectorGetElement(GetVector(heap), index_to_swap)) > 0)
	{
		index_to_swap = right_child_index;
	}

	return index_to_swap;
}

static dvector_t *GetVector(heap_t *heap)
{
	return heap->vector;
}


void Print(heap_t *heap)
{
	size_t n = HeapSize(heap);
	size_t i = 0;
	void *data = NULL;

	for (; i < n; ++i)
	{
		data = *(void **)DvectorGetElement(heap->vector, i);
		printf("%d -> ", *(int *)data);
	}
	printf("\n");
}


