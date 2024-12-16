/*
Code by: Or Yamin
Project: Hash table data structure
Date: 26/06/2024
Review by: Shahar Hadad
Review date: 29/06/2024
Approved by: Shahar Hadad
Approved date: 30/06/2024
*/

#include <assert.h> /* assert */
#include <stdlib.h> /* size_t malloc free */
#include <math.h> /* sqrt */

#include "hashtable.h"
#include "dllist.h"

struct hash_table
{
	dllist_t **hash_array;
	size_t hash_table_size;
	hash_func_t hash_func;
	hash_cmp_func_t cmp_func;
};

static void DestroyArray(dllist_t **array, size_t size);

hash_table_t *HashTableCreate(hash_func_t hash_func, hash_cmp_func_t cmp_func, size_t hash_table_size)
{
	hash_table_t *hash = NULL;
	size_t i = 0;
	dllist_t **runner = NULL;
	
	assert(NULL != hash_func);
	assert(NULL != cmp_func);
	assert(0 != hash_table_size);
	
	hash = (hash_table_t *)malloc(sizeof(struct hash_table));
	if(NULL == hash)
	{
		return NULL;
	}
	
	hash->hash_array = (dllist_t **)calloc(hash_table_size, sizeof(dllist_t *));
	if(NULL == hash->hash_array)
	{
		free(hash);
		return NULL;
	}
	
	runner = (dllist_t **)hash->hash_array;
	for(; i < hash_table_size; ++i)
	{
		*runner = DllistCreate();
		if(NULL == *runner)
		{
			DestroyArray(runner, i);
			free(hash->hash_array);
			free(hash);
			return NULL;
		}
		++runner;
	}
	
	hash->hash_table_size = hash_table_size;
	hash->hash_func = hash_func;
	hash->cmp_func = cmp_func;
	
	return hash;
}

void HashTableDestroy(hash_table_t *table)
{
	size_t i = 0;
	dllist_t **runner = NULL;
	
	assert(NULL != table);
	assert(NULL != table->hash_array);

	runner = (dllist_t **)table->hash_array;
	for(; i < table->hash_table_size; ++i)
	{	
		DllistDestroy(*runner);
		++runner;
	}	
	
	free(table->hash_array);
	free(table);
}


int HashTableInsert(hash_table_t *table, const void *key, void *data)
{
	size_t index = 0;
	dllist_iter_t iter = NULL;
	
	assert(NULL != table);
	assert(NULL != table->hash_func);
	assert(NULL != table->hash_array);
	assert(NULL != key);
	
	index = table->hash_func(key) % table->hash_table_size;
	iter = DllistGetBegin(table->hash_array[index]);
	
	iter = DllistInsertBefore(table->hash_array[index], data, iter);
	if(DllistIsSameIter(iter, DllistGetEnd(table->hash_array[index])))
	{
		return 1;
	}
	
	return 0;
}


void HashTableRemove(hash_table_t *table, const void *key)
{
	size_t index = 0;
	dllist_iter_t iter = NULL;
	
	assert(NULL != table);
	assert(NULL != table->cmp_func);
	assert(NULL != table->hash_array);
	assert(NULL != key);
	
	index = table->hash_func(key) % table->hash_table_size;
	
	iter = DllistFind((void *)key, DllistGetBegin(table->hash_array[index]),
						   DllistGetEnd(table->hash_array[index]),
						   table->cmp_func);
						   
	if(!(DllistIsSameIter(iter, DllistGetEnd(table->hash_array[index]))))
	{
		DllistRemove(iter);
	}
}


size_t HashTableSize(const hash_table_t *table)
{
	size_t count = 0;
	size_t i = 0;
	dllist_t **runner = NULL;
	
	assert(NULL != table);
	assert(NULL != table->hash_array);
	
	runner = (dllist_t **)table->hash_array;
	for(; i < table->hash_table_size; ++i)
	{
		count += DllistSize(*runner);
		++runner;
	}
	
	return count;
}


int HashTableIsEmpty(const hash_table_t *table)
{
	dllist_t **runner = NULL;
	size_t num_of_buckets = 0;
	
	assert(NULL != table);
	assert(NULL != table->hash_array);
	
	runner = (dllist_t **)table->hash_array;
	num_of_buckets = table->hash_table_size;
	while(num_of_buckets && DllistIsEmpty(*runner))
	{
		++runner;
		--num_of_buckets;
	}
	
	return (0 == num_of_buckets);
}


void *HashTableFind(const hash_table_t *table, const void *key)
{
	size_t index = 0;
	dllist_iter_t iter = NULL;
	dllist_iter_t cache_iter = NULL;
	void *data = NULL;
	
	assert(NULL != table);
	assert(NULL != table->cmp_func);
	assert(NULL != table->hash_array);
	assert(NULL != key);
	
	index = table->hash_func(key) % table->hash_table_size;
	
	iter = DllistFind((void *)key, DllistGetBegin(table->hash_array[index]),
						   DllistGetEnd(table->hash_array[index]),
						   table->cmp_func);
						   
	if((DllistIsSameIter(iter, DllistGetEnd(table->hash_array[index]))))
	{
		return NULL;
	}
	
	if(DllistIsSameIter(iter, DllistGetBegin(table->hash_array[index])))
	{
		return DllistGetData(iter);
	}
	
	/* caching */
	data = DllistGetData(iter);
	cache_iter = DllistGetBegin(table->hash_array[index]);
	cache_iter = DllistInsertBefore(table->hash_array[index], data, cache_iter);
	if(!(DllistIsSameIter(cache_iter, DllistGetEnd(table->hash_array[index]))))
	{
		DllistRemove(iter);
	}
	
	return data;
}


int HashTableForEach(hash_table_t *table, hash_action_func_t action, void *params)
{
	int status = 0;
	size_t i = 0;
	dllist_t **runner = NULL;
	
	assert(NULL != table);
	assert(NULL != table->hash_array);
	assert(NULL != action);
	
	runner = (dllist_t **)table->hash_array;
	for(; 0 == status && i < table->hash_table_size; ++i)
	{	
		status = DllistForEach(DllistGetBegin(*runner),
					  DllistGetEnd(*runner),
					  action, params);
					  
					  ++runner;
	}
	
	return status;
}

double HashTableStandardDeviation(const hash_table_t *table)
{
	double load = 0;
	double stddev = 0;
	double size = 0;
	double variance = 0;
	size_t i = 0;
	
	dllist_t **runner = NULL;
	assert(NULL != table);
	
	load = HashTableLoad(table);
	runner = (dllist_t **)table->hash_array;
	
	for(i = 0; i < table->hash_table_size; ++i)
	{
		size = (double)DllistSize(*runner);
		variance += (size - load) * (size - load);
		++runner;
	}
	
	variance /= table->hash_table_size;
	stddev = sqrt(variance);
	
	return stddev;
}

double HashTableLoad(const hash_table_t *table)
{
	size_t num_of_elements = 0;
	
	assert(NULL != table);
	
	num_of_elements = HashTableSize(table);
	
	return (double)num_of_elements / table->hash_table_size;
}

static void DestroyArray(dllist_t **array, size_t size)
{
	size_t i = 0;
	for(; i < size; ++i)
	{
		DllistDestroy(array[i]);
	}
}

