#include <assert.h> /*assert*/
#include <stddef.h>/*size_t*/
#include <stdio.h> /* printf */
#include <stdlib.h>/* malloc free */

#include "sorting.h"

static int Max(int arr[], size_t size);
static int Min(int arr[], size_t size);
static void CountSort(int arr[], int n, int exp);
void printarray(int arr[], int size);
static int MergeSortHelper(int *arr_to_sort, size_t low, size_t high);
static int MergeSubArrayes(int *arr_to_sort, size_t low, size_t mid, size_t high);
void SwapQuick(void *data1, void *data2, size_t size);
static size_t Partition(void *base, size_t low, size_t high, size_t size, int (*compar)(const void *, const void *));
static void Heapify(int arr[], size_t n, int index);
void HeapSwap(int *a, int *b);

void BubbleSort(int arr[], size_t n) 
{
	size_t i = 0, j = 0;
	int temp = 0;
	
	for (i = 0; i < n-1; i++)
	{
		for (j = 0; j < n-i-1; j++) 
		{
			if (arr[j] > arr[j+1]) 
			{
				temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;
			}
		}
	}
}

void SelectionSort(int arr[], size_t n) 
{
	size_t min_idx = 0;
	size_t i = 0, j = 0;
	int temp = 0;
	for (i = 0; i < n-1; i++) 
	{
		min_idx = i;
		for (j = i+1; j < n; j++) 
		{
			if (arr[j] < arr[min_idx]) 
			{
				min_idx = j;
			}
		}
		temp = arr[min_idx];
		arr[min_idx] = arr[i];
		arr[i] = temp;
    }
}

void InsertionSort(int arr[], size_t n) 
{
	size_t i = 0;
	int j = 0;
	int key = 0;
	
	for (i = 1; i < n; i++) 
	{
		key = arr[i];
		j = i - 1;
		while (j >= 0 && arr[j] > key) 
		{
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = key;
	}
}

void CountingSort(int arr[], size_t n)
{
	int max = Max(arr, n);
	int min = Min(arr, n);
	int *lut = (int *)calloc(1, ((max - min + 1) * sizeof(int)));
	size_t i = 0, j = 0;
	size_t count = 0;
	size_t index = 0;
	int element = 0;
	
	for(; i < n; ++i)
	{
		index = arr[i];
		lut[index - min] += 1;
	}
	
	
	for(i = 0, j = 0; j < (size_t)(max - min + 1); ++j)
	{
		count = lut[j];
		element = j + min;
		while(1 <= count)
		{
			arr[i] = element;
			++i;
			--count;
		}
	}
	free(lut);
}

void RadixSort(int arr[], size_t n)
{
    int max = Max(arr, n);
	int exp = 0;
	
    for (exp = 1; max / exp > 0; exp *= 10)
    {
        CountSort(arr, n, exp);
    }
}

size_t BinarySearch(int arr[], size_t size, int target)
{
	size_t low = 0;
	size_t mid = size / 2;
	size_t high = size - 1;
	
	while(low <= high)
	{
		mid = low + (high - low) / 2;
		
		if(target == arr[mid])
		{
			return mid;
		}
		
		if(target < arr[mid])
		{
			high = mid - 1;
		}
		else
		{
			low = mid + 1;
		}
	}
	
	return -1;
}

size_t RecursiveBinarySearch(int arr[], size_t low, size_t high, int target)
{
	size_t mid = low + (high - low) / 2;
	
	if(low > high)
	{
		return -1;
	}
	
	if(target == arr[mid])
	{
		return mid;
	}
	
	if(target < arr[mid])
	{
		return RecursiveBinarySearch(arr, low, mid - 1, target);
	}
	else
	{
		return RecursiveBinarySearch(arr, mid + 1, high, target);
	}
}

int MergeSort(int *arr_to_sort, size_t num_elements)
{
	size_t low = 0;
	size_t high = num_elements - 1;
	return MergeSortHelper(arr_to_sort, low, high);
}

static int MergeSortHelper(int *arr_to_sort, size_t low, size_t high)
{
	size_t mid = 0;

	if(low >= high)
	{
		return 0;
	}
	
	mid = low + (high - low) / 2;
	MergeSortHelper(arr_to_sort, low, mid);
	MergeSortHelper(arr_to_sort, mid + 1, high);
	
	return MergeSubArrayes(arr_to_sort, low, mid, high);
}

void Qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{	
	size_t pivot = 0;
	if (nmemb < 2)
	{
		return;
	}
	
	pivot = Partition(base, 0, nmemb - 1, size, compar);
	
	Qsort(base, pivot, size, compar);
	Qsort((char*)base + (pivot + 1) * size, nmemb - pivot - 1, size, compar);
}

static size_t Partition(void *base, size_t low, size_t high, size_t size, int (*compar)(const void *, const void *))
{
	char *char_base = (char*)base;
	size_t pivot_index = high;
	size_t i = low;
	size_t j = low;
	
	for ( j = low; j < high; ++j)
	{
		if (compar(char_base + j * size, char_base + pivot_index * size) < 0)
		{
			SwapQuick(char_base + i * size, char_base + j * size, size);
			++i;
		}
	}
	SwapQuick(char_base + i * size, char_base + pivot_index * size, size);
	
	return i;
}

void HeapSort(int arr[], size_t n)
{
	int i = n / 2 - 1; /*last non-leaf node*/
	/* create Max-Heap */
	for(; i >= 0; --i)
	{
		Heapify(arr, n, i);
	}
	
	/* heap sort the array */
	for(i = n -1; i >= 0; --i)
	{
		HeapSwap(&arr[i], &arr[0]);
		
		Heapify(arr, i, 0);
	}
}

static void Heapify(int arr[], size_t n, int index)
{
	int left_child = 2 * index + 1;
	int right_child = 2 * index + 2;	
	int largest = index;
	
	if(left_child < (int)n && arr[left_child] > arr[largest])
	{
		largest = left_child;
	}
	
	if(right_child < (int)n && arr[right_child] > arr[largest])
	{
		largest = right_child;
	}
	
	if(index != largest)
	{
		HeapSwap(&arr[index], &arr[largest]);
		
		Heapify(arr, n, largest);
	}	
}

void HeapSwap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;	
}

/**********************************HELPERS************************************/
static int Min(int arr[], size_t size)
{
	size_t i = 0;
	int min = arr[i];
	
	for(i = 1; i < size; ++i)
	{
		if(arr[i] < min)
		{
			min = arr[i];
		}
	}
	
	return min;
}


static int Max(int arr[], size_t size)
{
	size_t i = 0;
	int max = arr[i];
	
	for(i = 1; i < size; ++i)
	{
		if(arr[i] > max)
		{
			max = arr[i];
		}
	}
	
	return max;
}

static void CountSort(int arr[], int n, int exp)
{

   /*  Output array*/
	int *output = (int *)calloc(1, (sizeof(int) * (n + 1)));
	int i, count[10] = { 0 };

	/* Store count of occurrences
	in count[]*/
	for (i = 0; i < n; i++)
	{
		count[(arr[i] / exp) % 10]++;
	}

	/* Change count[i] so that count[i]
	now contains actual position
	of this digit in output[]*/
	for (i = 1; i < 10; i++)
	{
		count[i] += count[i - 1];
	}
    
	/* Build the output array*/
	for (i = n - 1; i >= 0; i--) 
	{
		output[count[(arr[i] / exp) % 10] - 1] = arr[i];
		count[(arr[i] / exp) % 10]--;
    }

	/*Copy the output array to arr[],
	so that arr[] now contains sorted
	numbers according to current digit*/
	for (i = 0; i < n; i++)
	{
		arr[i] = output[i];
	}
        
    free(output);
}

void printarray(int arr[], int size) 
{
	size_t i = 0;
	
	for (i = 0; i < (size_t)size; i++) 
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
}

static int MergeSubArrayes(int *arr_to_sort, size_t low, size_t mid, size_t high)
{
	size_t i = 0, j = 0, k = low;
	size_t temp_low_arr_size = mid - low + 1;
	size_t temp_high_arr_size = high - mid;
	int *temp_low_arr = NULL;
	int *temp_high_arr = NULL;
	
	temp_low_arr = (int *)malloc(sizeof(int) * (temp_low_arr_size));
	if(NULL == temp_low_arr)
	{
		return 1;
	}
	
	temp_high_arr = (int *)malloc(sizeof(int) * (temp_high_arr_size));
	if(NULL == temp_high_arr)
	{
		free(temp_low_arr);
		return 1;
	}
	
	for (i = 0; i < temp_low_arr_size; i++)
	{
		temp_low_arr[i] = arr_to_sort[low + i];
	}
		
	for (j = 0; j < temp_high_arr_size; j++)
	{
		temp_high_arr[j] = arr_to_sort[mid + 1 + j];
	}
	
	i = 0, j = 0;
	while (i < temp_low_arr_size && j < temp_high_arr_size) 
	{
		if (temp_low_arr[i] <= temp_high_arr[j]) 
		{
			arr_to_sort[k] = temp_low_arr[i];
			i++;
		}
		else 
		{
			arr_to_sort[k] = temp_high_arr[j];
			j++;
		}
		k++;
	}
	
	while (i < temp_low_arr_size) 
	{
		arr_to_sort[k] = temp_low_arr[i];
		i++;
		k++;
    }

	while (j < temp_high_arr_size) 
	{
		arr_to_sort[k] = temp_high_arr[j];
		j++;
		k++;
	}		
	
	free(temp_low_arr);
	free(temp_high_arr);
	
	return 0; 
}

void SwapQuick(void *data1, void *data2, size_t size)
{
	size_t i = 0;
	char temp = '\0';
	char *runner1 = (char *)data1;
	char *runner2 = (char *)data2;
	
	for(; i < size; ++i)
	{
		temp = *runner1;
		*runner1 = *runner2;
		*runner2 = temp;
		++runner1;
		++runner2;
	}
}

