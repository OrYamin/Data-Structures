/*
Reviewed by Amit
written by Or Yamin on 19/05/2024
*/
#include <stdlib.h> /* size_t malloc() free() */
#include <assert.h> /* assert() */

#include "dllist.h"


struct dllist_node 
{
    void *data;
    struct dllist_node *next;
    struct dllist_node *prev;
};

struct dllist 
{
    struct dllist_node *head;
    struct dllist_node *tail;
};



static dllist_iter_t CreateNode(dllist_iter_t next, dllist_iter_t prev, void* data);
static int counter(void *data, void *param);


dllist_t *DllistCreate(void)
{
	dllist_t *list = (dllist_t *)malloc(sizeof(dllist_t));
	dllist_iter_t dummy_head = NULL;
	dllist_iter_t dummy_tail = NULL;
	
	if (NULL == list) 
	{
		return NULL;
	}
	
	dummy_head = CreateNode(NULL, NULL, NULL);
	if (dummy_head == NULL) 
	{
		free(list);
		return NULL;
	}

	dummy_tail = CreateNode(NULL, dummy_head, NULL);
	if (dummy_tail == NULL) 
	{
		free(dummy_head);
		free(list);
		return NULL;
	}
	dummy_head->next = dummy_tail;


	list->tail = dummy_tail;
	list->head = dummy_head;
	

    return list;
}



void DllistDestroy(dllist_t *list)
{
	dllist_iter_t current = NULL;
	dllist_iter_t next = NULL;
	assert(NULL != list);

	current = list->head;
	while (NULL != current) 
	{
		next = current->next; 
		free(current);
		current = next; 
	}
	
	free(list);
	list = NULL;
}



dllist_iter_t DllistNext(dllist_iter_t iter)
{
	assert(iter->next != NULL); 
	return iter->next;
}



dllist_iter_t DllistPrev(dllist_iter_t iter)
{
	assert(iter->prev->prev != NULL); 
	return iter->prev;
}



dllist_iter_t DllistGetBegin(const dllist_t *list)
{
	assert(list != NULL);
	return list->head->next;
}



dllist_iter_t DllistGetEnd(const dllist_t *list)
{
	assert(list != NULL);
	return list->tail; 
}



dllist_iter_t DllistInsertBefore(dllist_t *list, void *data, dllist_iter_t where)
{
	dllist_iter_t new_node = NULL;
	assert(NULL != list && NULL != where);
	
	
	new_node = CreateNode(where, where->prev, data);
	if(NULL == new_node)
	{
		return NULL;
	}
	
					
	where->prev->next = new_node;
	where->prev = new_node;
	return new_node;
}



dllist_iter_t DllistRemove(dllist_iter_t where)
{
	dllist_iter_t temp = NULL;
	assert(NULL != where && NULL != where->next);
	
	temp = where->next;
	where->prev->next = where->next;
    where->next->prev = where->prev;
    
    free(where);
    return temp;	
}	
		


size_t DllistSize(const dllist_t *list)
{		
    size_t count = 0;
    dllist_iter_t from = NULL;
    dllist_iter_t to = NULL;
    assert(NULL != list);
    from = DllistGetBegin(list);
    to = DllistGetEnd(list);
    DllistForEach(from, to, counter, &count);
    
    return count;
}



void *DllistGetData(dllist_iter_t iter)
{
	assert(NULL != iter);
    return iter->data;
}


dllist_iter_t DllistSetData(dllist_iter_t iterator, void *new_data)
{
    assert(iterator != NULL);
	iterator->data = new_data;
    return iterator;
}



dllist_iter_t DllistFind(void *param, dllist_iter_t from, dllist_iter_t to, match_func_t is_match)
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



int DllistIsSameIter(dllist_iter_t iter1, dllist_iter_t iter2)
{
	assert(NULL != iter1 && NULL != iter2);
    return (iter1 == iter2);
}



int DllistIsEmpty(const dllist_t *list)
{
	assert( NULL != list);
    return (list->head->next == list->tail);
}



int DllistForEach(dllist_iter_t from, dllist_iter_t to, action_func_t action, void *param)
{
	int status = 0;
	assert( NULL != from && NULL != to);
    while (from != to) 
    {
    	status = action(from->data, param);
        from = from->next;
    }
    return status; 
}
	
	

dllist_iter_t DllistSplice(dllist_iter_t where, dllist_iter_t from, dllist_iter_t to)
{
	dllist_iter_t before_from = NULL;
	assert(NULL != where && NULL != to && NULL != from);
	
	before_from = from->prev;
	where->prev->next = from;
	from->prev->next = to;
	
	to->prev->next = where;
	from->prev = where->prev;
	
	where->prev = to->prev;
	to->prev = before_from;
	
	return where->prev;
}





dllist_iter_t DllistPushFront(dllist_t *list, void *data)
{
	assert(NULL != list);
	return DllistInsertBefore(list, data, DllistGetBegin(list));
}



dllist_iter_t DllistPushBack(dllist_t *list, void *data)
{	
	assert(NULL != list);
	return DllistInsertBefore(list, data, list->tail);
}



void *DllistPopFront(dllist_t *list)
{
	void *data = NULL;
	assert(NULL != list);
	
	data = list->head->next->data;
	DllistRemove(DllistGetBegin(list));
	return data;		
}



void *DllistPopBack(dllist_t *list)
{
	void *data = NULL;
	assert(NULL != list);
	
	data = list->tail->prev->data;
	DllistRemove(list->tail->prev);
	return data;		
}



int DllistMultiFind(dllist_iter_t from, dllist_iter_t to, match_func_t is_match, void *param, dllist_t *output)
{	
	int count = 0;
	dllist_iter_t iter = NULL;
	
	assert(NULL != from && NULL != to);
	
	while(from != to)
	{
		if (is_match(from->data, param))
		{
			iter = DllistPushBack(output, from->data);
			if (NULL == iter)
			{
				return -1;
			}
			++count;
		}
		from = from->next;
	}
	return count;
}




static dllist_iter_t CreateNode(dllist_iter_t next, dllist_iter_t prev, void* data)
{
	dllist_iter_t node = (dllist_iter_t)malloc(sizeof(struct dllist_node));
	if (node == NULL) 
	{
        return NULL; 
	}
	
	node->data = data;
	node->next = next;
	node->prev = prev;
	
	return node;
}





static int counter(void *data, void *param)
{
	(void)data;
	++(*(size_t *)param);
	return 1;
}

