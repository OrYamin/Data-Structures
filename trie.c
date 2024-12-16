#include <stdlib.h> /* size_t malloc free */
#include <assert.h> /* assert */

#include "trie.h"

#define BINARY_RANGE 2
#define ZERO 0
#define ONE 1
#define FULL 1
#define NOT_FULL 0

#define CHECK_CHILDREN_FULL_FLAGS(node) (IsFull(node->children[ZERO]) && \
										IsFull(node->children[ONE])) ? 1 : 0

typedef struct trie_node
{
	struct trie_node *children[BINARY_RANGE];
	struct trie_node *parent;
	int is_full;
}trie_node_t;

struct trie
{
	trie_node_t *root;
	size_t depth;
};

static trie_node_t *TrieNodeCreate(trie_node_t *parent);
static void TrieNodeDestroy(trie_node_t *node);
static void SetZerosFromRight(host_id_t *host_id, size_t index);
static size_t HelperTrieCount(trie_node_t *node, size_t height);
static int IsFull(trie_node_t *node);
static void UpdateFullness(trie_node_t *node);
static host_id_t SetBitOn(host_id_t data, size_t index);
static host_id_t SetBitOff(host_id_t data, size_t index);
static host_id_t SetBit(host_id_t data, size_t index, size_t bool_value);


/*************************************API functions ***************************/

trie_t *TrieCreate(size_t depth)
{
	trie_t *trie = (trie_t *)malloc(sizeof(trie_t));
	if (NULL == trie)
	{
		return NULL;
	}
	
	trie->root = TrieNodeCreate(NULL);
	if (NULL == trie->root)
	{
		free(trie);
		return NULL;
	}
	
	trie->depth = depth;
	
	return trie;
}

void TrieDestroy(trie_t *trie)
{
	assert(NULL != trie);
	
	TrieNodeDestroy(trie->root);
	free(trie);
}

int TrieInsert(trie_t *trie, host_id_t host_id, host_id_t *recived_host_id)
{
	trie_node_t *current = NULL;
	trie_node_t *child = NULL;
	size_t index = 0;
	size_t depth = 0;
	host_id_t mask = 0;

	assert(recived_host_id);
	assert(trie);
	
	mask = 1 << (trie->depth - 1);
	current = trie->root;
	
	if(IsFull(current))
	{
		return TRIE_FULL;
	}
	
	while (depth < trie->depth)
	{
		index = (mask == (host_id & mask));
		child = current->children[index];
		
		/* if child is not full, step its way */
		if(!IsFull(child))
		{
			if (NULL == child) /* if child is not exist, first create him */
			{
				child = TrieNodeCreate(current);
				if(NULL == child)
				{
					return TRIE_MEMORY_FALIURE;
				}
				current->children[index] = child;			
			}
			
			current = child;
			++depth;
			mask >>= 1;
			*recived_host_id = SetBit(*recived_host_id, trie->depth - depth, index);
		}
		else /* if subtree is full, set host_id to be the next possible address*/
		{
			host_id += mask;
			SetZerosFromRight(&host_id, trie->depth - depth - 1);
			
			return TrieInsert(trie, host_id, recived_host_id);	
		}
	}
	
	current->is_full = FULL;
	UpdateFullness(current->parent);

	return TRIE_SUCCESS; 
}

int TrieRemove(trie_t *trie, host_id_t host_id)
{
	trie_node_t *current = NULL;
	trie_node_t *child = NULL;
	size_t index = 0;
	size_t depth = 0;
	host_id_t mask = 0;

	assert(trie);
	
	mask = 1 << (trie->depth - 1);
	current = trie->root;
	child = current;
	
	while (depth < trie->depth && NULL != child)
	{
		index = (mask == (host_id & mask));
		child = current->children[index];
		
		current = child;
		++depth;
		mask >>= 1;
	}
	
	if (NULL == child)
	{
		return TRIE_DOUBLE_FREE;
	}
	
	
	child->parent->children[index] = NULL;
	current->is_full = NOT_FULL;
	UpdateFullness(current->parent);
	free(current);
	
	return TRIE_SUCCESS;
}

size_t TrieCountIps(const trie_t *trie)
{
	assert(NULL != trie);
	
	return HelperTrieCount(trie->root, trie->depth);
}


/********************************* helpers ***********************************/

static size_t HelperTrieCount(trie_node_t *node, size_t height)
{
	if (NULL == node)
	{
		return 0;
	}
	
	if (IsFull(node))
	{
		return 1 << height;
	}
	
	return HelperTrieCount(node->children[ZERO], height-1) + 
				HelperTrieCount(node->children[ONE], height-1);
}

static void UpdateFullness(trie_node_t *node)
{
	assert(NULL != node);
	
	while(NULL != node)
	{
		node->is_full = CHECK_CHILDREN_FULL_FLAGS(node);
		node = node->parent;
	}
}

static host_id_t SetBitOn(host_id_t data, size_t index)
{
	return (data | (host_id_t)1 << index);
}

static host_id_t SetBitOff(host_id_t data, size_t index)
{
	return (data & ~((host_id_t)1 << index));
}

static host_id_t SetBit(host_id_t data, size_t index, size_t bool_value)
{	
	if(bool_value)
	{
		return SetBitOn(data, index);
	}
	return SetBitOff(data, index);
}

static trie_node_t *TrieNodeCreate(trie_node_t *parent)
{
	trie_node_t *node = (trie_node_t *)malloc(sizeof(trie_node_t));
	if(NULL != node)
	{
		node->is_full = 0;
		node->children[ZERO] = NULL;
		node->children[ONE] = NULL;
		node->parent = parent;
	}
	
	return node;
}

static int IsFull(trie_node_t *node)
{
	return (NULL == node) ? NOT_FULL : node->is_full;
}

static void SetZerosFromRight(host_id_t *host_id, size_t index)
{	
	*host_id &= (~0 << index);
}

static void TrieNodeDestroy(trie_node_t *node)
{
	if (NULL != node)
	{
		TrieNodeDestroy(node->children[ZERO]);
		TrieNodeDestroy(node->children[ONE]);
		free(node);
	}
}

