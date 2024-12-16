/*approved by Shahar Hadad
written by Or Yamin on 10/05/2024*/

#include <stdlib.h> /* size_t malloc() free() */
#include <assert.h> /* assert() */

#include "sllist.h"


struct sllist_node 
{
    void *data;
    struct sllist_node *next;
};

struct sllist 
{
    struct sllist_node *head;
    struct sllist_node *tail;
};

static int counter(void *data, void *param);
static int IsDummy(sllist_iter_t iter);
static sllist_iter_t CreateNode(sllist_iter_t next, void* data);

sllist_t *SllistCreate(void) 
{
	sllist_t *list = (sllist_t *)malloc(sizeof(sllist_t));
	sllist_iter_t dummy = NULL;
	if (NULL == list) 
	{
		return NULL;
	}
	
	dummy = CreateNode(NULL, list);
	if (dummy == NULL) 
	{
		return NULL;
	}

	list->tail = dummy;
	list->head = dummy;
	

    return list;
}


void SllistDestroy(sllist_t *sllist)  
{
	sllist_iter_t current = NULL;
	sllist_iter_t next = NULL;
	
	if (NULL == sllist) 
	{
		return;
	}

	current = sllist->head;
	while (NULL != current ) 
	{
		next = current->next; 
		free(current);
		current = next; 
	}

	free(sllist);
	sllist = NULL;
}



sllist_iter_t SllistNext(sllist_iter_t iter) 
{
	assert(iter->next != NULL); 
	return iter->next;
}


sllist_iter_t SllistBegin(const sllist_t *sllist) 
{
	assert(sllist != NULL);
	return sllist->head;
}


sllist_iter_t SllistEnd(const sllist_t *sllist) 
{
	assert(sllist != NULL);
	return sllist->tail; 
}



sllist_iter_t SllistInsertBefore(sllist_t *sllist, void *data, sllist_iter_t where) 
{
	struct sllist_node *new_node;
	assert(sllist != NULL && data != NULL); 
	new_node = CreateNode(where->next, where->data);
	
	if (new_node == NULL) 
	{
        return NULL; 
	}

	if(IsDummy(where))
	{
		sllist->tail = new_node;
	}

	where->data = data;
	where->next = new_node;
	

	return where;
}


sllist_iter_t SllistRemove(sllist_iter_t where)  
{

	sllist_iter_t temp = where->next;
    assert(where != NULL);
    where->data = where->next->data;
    where->next = where->next->next;
    
    if (IsDummy(where))
	{
		((sllist_t *)(where -> data)) -> tail = where;
	}

    free(temp);
    return where;
}


size_t SllistCount(const sllist_t *sllist)
{
	
    size_t count = 0;
    sllist_iter_t from = NULL;
    sllist_iter_t to = NULL;
    assert(NULL != sllist);
    from = SllistBegin(sllist);
    to = SllistEnd(sllist);
    SllistForEach(from, to, counter, &count);
    
    return count;
}


void *SllistGetData(sllist_iter_t iterator) 
{
	assert(NULL != iterator);
    return iterator->data;
}


sllist_iter_t SllistSetData(sllist_iter_t iterator, void *new_data)
{
    assert(iterator != NULL);
	iterator->data = new_data;
    return iterator;
}


sllist_iter_t SllistFind(sllist_iter_t from, sllist_iter_t to, match_func_t is_match, void *param) 
{
	assert(NULL != from && NULL != to);
    while (from != to ) 
    {
        if (is_match(from->data, param)) 
        {
            return from;
        }
        from = from->next;
    }

    return to;
}


int SllistIterIsEqual(sllist_iter_t iterator1, sllist_iter_t iterator2) 
{
	assert(NULL != iterator1 && NULL != iterator2);
    return (iterator1 == iterator2);
}


int SllistIsEmpty(const sllist_t *sllist) 
{
	assert( NULL != sllist);
    return (sllist->head == sllist->tail);
}


int SllistForEach(sllist_iter_t from, sllist_iter_t to, action_func_t action, void *param)
{
	int status = 0;
	assert( NULL != from && NULL != to);
    while (from != to) 
    {
    	status = action(from->data, param);
        if (status == 0) 
        {
            return -1; 
        }
        from = from->next;
    }
    return status; 
}



void SllistAppend(sllist_t *dest, sllist_t *src)
{
	assert(NULL != dest && NULL != src);
	dest->tail->data = src->head->data;
	dest->tail->next = src->head->next;
	
	dest->tail = src->tail;
	dest->tail->data = dest;
	
	src->head->next = NULL;
	src->head->data = src;
	src->tail = src->head;
}


static int counter(void *data, void *param)
{
	(void)data;
	++(*(size_t *)param);
	return 1;
}


static int IsDummy(sllist_iter_t iter)
{
	assert( NULL != iter);
	return (NULL == iter -> next);
}


static sllist_iter_t CreateNode(sllist_iter_t next, void* data)
{
	sllist_iter_t node = (sllist_iter_t)malloc(sizeof(struct sllist_node));
	if (node == NULL) 
	{
        return NULL; 
	}
	
	node->data = data;
	node->next = next;
	
	return node;
}

