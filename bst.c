/*
Code by: Or Yamin
Project: BST data structure
Date: 10/06/2024
Review by: Shahar Marom
Review date: 17/06/2024
Approved by: Shahar Marom
Approved date: 17/06/2024
*/

#include <stdlib.h> /*malloc free size_t*/
#include <assert.h> /*assert*/

#include "bst.h"

#define LEFT child[0]
#define RIGHT child[1]
#define NEXT 1
#define PREV 0
#define NUM_OF_CHILDRENS 2

#define WHERE_TO_GO(x) (x < 0) ? 1 : 0

struct bst_node
{
	void *data;
	node_t child[NUM_OF_CHILDRENS];
	node_t parent;
};

struct bst
{
	node_t root;
	bst_cmp_func_t cmp_func;
};


static int Counter(void *data, void *param);
static node_t CreateNode(void *data, node_t parent);
static int IsIterLeftChild(bst_iter_t parent, bst_iter_t iter);
static int IsLeaf(bst_iter_t iter);
static bst_iter_t NextPrev(bst_iter_t iter, int where);
static int HasRightOrLeftChild(bst_iter_t iter);
static int HasOnlyOneChild(bst_iter_t iter);
static node_t IterToNode(bst_iter_t iter);
static bst_iter_t NodeToIter(node_t iter);
static int HasLeftChild(bst_iter_t iter);
static int HasRightChild(bst_iter_t iter);
static void DetachParent(node_t iter_to_remove, node_t new_child);


bst_t *BSTCreate(bst_cmp_func_t compare)
{
	bst_t *tree = NULL;
	assert(NULL != compare);
	
	tree = (bst_t *)malloc(sizeof(bst_t));
	if(NULL == tree)
	{
		return NULL;
	}
	
	tree->root = CreateNode(NULL, NULL);
	if(NULL == tree->root)
	{
		free(tree);
		return NULL;
	}
	
	tree->cmp_func = compare;
	
	return tree;
}

void BSTDestroy(bst_t *tree)
{
	node_t runner = NULL;
	node_t parent = NULL;
	
	assert(NULL != tree);
	
	runner = tree->root->LEFT;
	
	while (NULL != runner && !BSTIsSameIter(BSTEnd(tree), NodeToIter(runner)))
	{
		if (HasLeftChild(runner))
		{
			runner = runner->LEFT;
		}
		else if (HasRightChild(runner))
		{
			runner = runner->RIGHT;
		}
		else
		{
			parent = runner->parent;
			BSTRemove(runner);
			runner = parent;
		}
	}
	
	free(tree->root);
	free(tree);
}


int BSTIsEmpty(const bst_t *tree)
{
	assert(NULL != tree);
	assert(NULL != tree->root);
	
	return (NULL == tree->root->LEFT);
}



size_t BSTSize(const bst_t *tree)
{
	size_t count = 0;
	
	assert(NULL != tree);
	
	BSTForEach((bst_t *)tree, Counter, &count);
	
	return count;
}



bst_iter_t BSTInsert(bst_t *tree, void *data)
{
	node_t runner = NULL;
	node_t new_node = NULL;
	node_t parent_of_runner = NULL;
	int result_from_cmp_func = 0;
	
	assert(NULL != tree);
	assert(NULL != tree->root);
	
	new_node = CreateNode(data, NULL);
	if(NULL == new_node)
	{
		return BSTEnd(tree);
	}
	
	parent_of_runner = tree->root;
	runner = tree->root->LEFT;
	
	while(NULL != runner)
	{
		parent_of_runner = runner;
		result_from_cmp_func = tree->cmp_func(BSTGetData(runner), data);
		runner = runner->child[WHERE_TO_GO(result_from_cmp_func)];
	}
	
	new_node->parent = parent_of_runner;
	parent_of_runner->child[WHERE_TO_GO(result_from_cmp_func)] = new_node;
	
	return NodeToIter(new_node);
}



void *BSTRemove(bst_iter_t iter)
{
	void *data = NULL;
	node_t next_iter = NULL;
	node_t iter_to_remove = NULL;
	assert(NULL != iter);
	
	iter_to_remove = IterToNode(iter);
	data = BSTGetData(iter_to_remove);

	if(IsLeaf(iter_to_remove))
	{
		DetachParent(iter_to_remove, NULL);
		free(iter_to_remove);
	}
	else if (HasOnlyOneChild(iter_to_remove))
	{
		int right_or_left = HasRightOrLeftChild(iter_to_remove);
		DetachParent(iter_to_remove, iter_to_remove->child[right_or_left]);

		free(iter_to_remove);
	}
	else
	{
		next_iter = BSTNext(iter_to_remove);
		
		iter_to_remove->data = next_iter->data;
		
		if (BSTIsSameIter(next_iter->parent, iter_to_remove))
		{
			iter_to_remove->RIGHT = next_iter->RIGHT;
			if (HasRightChild(next_iter))
			{
				next_iter->RIGHT->parent = iter_to_remove;
			}
		}
		else
		{
			next_iter->parent->LEFT = next_iter->RIGHT;
			if (HasRightChild(next_iter))
			{
				next_iter->RIGHT->parent = next_iter->parent;
			}
		}
		
		free(next_iter);
	}
	
	return data;
}
		


bst_iter_t BSTBegin(const bst_t *tree)
{
	node_t runner = NULL;
	
	assert(NULL != tree);
	assert(NULL != tree->root);

	runner = tree->root;
	while(HasLeftChild(runner))
	{
		runner = runner->LEFT;
	}
	
	return NodeToIter(runner);
}



bst_iter_t BSTEnd(const bst_t *tree)
{
	assert(NULL != tree);
	assert(NULL != tree->root);
	
	return NodeToIter(tree->root);
}



bst_iter_t BSTNext(bst_iter_t iter)
{
	assert(NULL != iter);
	return NextPrev(iter, NEXT);
}
	
	
	
bst_iter_t BSTPrev(bst_iter_t iter)
{
	assert(NULL != iter);
	return NextPrev(iter, PREV);
}	
	
	

int BSTIsSameIter(bst_iter_t iter1, bst_iter_t iter2)
{	
	return (iter1 == iter2);
}
	
	
	
void *BSTGetData(bst_iter_t iter)
{
	assert(NULL != iter);
	IterToNode(iter);
	
	return NodeToIter(iter)->data;
}



bst_iter_t BSTFind(const bst_t *tree, const void *data)
{	
	node_t runner = NULL;
	int result_from_cmp_func = 0;
	assert(NULL != tree);
	assert(NULL != tree->root);
	assert(NULL != tree->cmp_func);
	
	runner = tree->root->LEFT;
	
	while(NULL != runner)
	{
		result_from_cmp_func = tree->cmp_func(BSTGetData(runner), data);
		if (0 != result_from_cmp_func)
		{
			runner = runner->child[WHERE_TO_GO(result_from_cmp_func)];
		}
		else
		{
			return runner;
		}
	}
	
	return NodeToIter(BSTEnd(tree));
}



int BSTForEach(bst_t *tree, bst_action_func_t action, void* param)
{	
	node_t iter = NULL;
	int status = 0;

	assert(NULL != tree);
	assert(NULL != tree->root);
	assert(NULL != action);

	iter = BSTBegin(tree);
	
	while (!BSTIsSameIter(BSTEnd(tree), iter))
	{
		status = action(iter->data, param);
		iter = BSTNext(iter);
	}
	
	return status;
}







/**************************helpers****************************/
static void DetachParent(node_t iter_to_remove, node_t new_child)
{
	if(IsIterLeftChild(iter_to_remove->parent, iter_to_remove))
	{
		iter_to_remove->parent->LEFT = new_child;
	}
	else
	{
		iter_to_remove->parent->RIGHT = new_child;
	}
	
	if(NULL != new_child)
	{
		new_child->parent = iter_to_remove->parent;
	}
}


static int Counter(void *data, void *param)
{
	(void)data;
	++(*(size_t *)param);
	return 1;
}



static node_t CreateNode(void *data, node_t parent)
{
	node_t  new_node = (bst_iter_t)malloc(sizeof(struct bst_node));
	if(NULL == new_node)
	{
		return NULL;
	}
	
	new_node->data = data;
	new_node->parent = parent;
	new_node->LEFT = NULL;
    new_node->RIGHT = NULL;
	
	return new_node;
}
	
	
static bst_iter_t NextPrev(bst_iter_t iter, int where)
{
	node_t runner = NULL;
	node_t parent = NULL;
	assert(NULL != iter);
	
	runner = IterToNode(iter);
	if(NULL != runner->child[where])
	{
		runner = runner->child[where];
		while(NULL != runner->child[!where])
		{
			runner = runner->child[!where];
		}
	}
	else
	{
		parent = iter->parent;
		while(!BSTIsSameIter(runner, parent->child[!where]))
		{
			runner = parent;
			parent = parent->parent;
		}
		runner = parent;
	}
	
	return NodeToIter(runner);
}
	
static int IsIterLeftChild(bst_iter_t parent, bst_iter_t iter)
{
	assert(NULL != parent);
    assert(NULL != iter);
    
	return BSTIsSameIter(parent->LEFT, iter);
}



static int IsLeaf(bst_iter_t iter)
{
	assert(NULL != iter);
	
	return (NULL == iter->LEFT && NULL == iter->RIGHT);
}



static int HasOnlyOneChild(bst_iter_t iter)
{
	assert(NULL != iter);
	
	return (NULL == iter->LEFT) || (NULL == iter->RIGHT);
}

static int HasRightChild(bst_iter_t iter)
{
	assert(NULL != iter);
	
	return (NULL != iter->RIGHT);
}


static int HasLeftChild(bst_iter_t iter)
{
	assert(NULL != iter);
	
	return (NULL != iter->LEFT);
}	

static int HasRightOrLeftChild(bst_iter_t iter)
{
	assert(NULL != iter);
	
	if(NULL == iter->LEFT)
	{
		return 1;
	}
	
	return 0;
}


static node_t IterToNode(bst_iter_t iter)
{
	return iter;
}


static bst_iter_t NodeToIter(node_t iter)
{
	return iter;
}
