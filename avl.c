/* 
   Code by: Or Yamin
   Project: AVL - Binary search tree
   Date: 20/06/24
   Review by: Ido
   Review Date: 23/06/24
   Approved by: Ido
   Approval Date: 23/06/24
*/
//dddddddddddd
#include <assert.h> /*assert*/
#include <stdlib.h> /*malloc free size_t*/

#include "avl.h"

#define LEFT child[0]
#define RIGHT child[1]
#define ROTATE_LEFT 0
#define ROTATE_RIGHT 1
#define NUM_OF_CHILDREN 2
#define LEAF_HEIGHT 1
#define MAX(x, y) x > y ? x : y
#define WHERE_TO_GO(x) (x < 0) ? 1 : 0


struct avl
{
	struct avl_node *root;
	avl_cmp_func_t cmp_func;
};


typedef struct avl_node
{
	void *data;
	size_t height;
	struct avl_node *child[NUM_OF_CHILDREN];
}*node_t;


/* helper functions declaration */

static node_t CreateNode(void *data);
static void AVLDestroyNode(node_t node);
static size_t CountNodes(const node_t node);
static size_t NodeHeight(const node_t node);
static void UpdateHeight(node_t node);
static int GetBalance(node_t node);
static node_t AVLInsertNode(node_t node, node_t new_node, avl_cmp_func_t compare);
static node_t Rotate(node_t root, int side);
static node_t FixBalance(node_t node);
static node_t AVLFindNode(node_t node, const void *data, avl_cmp_func_t compare);
static int AVLForEachNode(node_t node, avl_action_func_t action, void* params);
static node_t MinValueNode(node_t node);
static node_t AVLRemoveNode(node_t root, void *data, avl_cmp_func_t compare);
static int NodeFound(int cmp_result);
static int HasOneOrNoChild(node_t node);
static int NodeIsLeaf(node_t node);

/**************************************** AVL ******************************/

avl_t *AVLCreate(avl_cmp_func_t compare)
{
	avl_t *tree = NULL;
	
	assert(NULL != compare);
	
	tree = (avl_t *)malloc(sizeof(struct avl));
	if(NULL == tree)
	{
		return NULL;
	}
	
	tree->root = NULL;
	tree->cmp_func = compare;
	
	return tree;
}



void AVLDestroy(avl_t *tree)
{
	assert(NULL != tree);
	
	AVLDestroyNode(tree->root);
	free(tree);
}


static void AVLDestroyNode(node_t node)
{
	if (node == NULL)
	{
		return;
	}

	AVLDestroyNode(node->LEFT);
	AVLDestroyNode(node->RIGHT);
	free(node);
}	
	


int AVLIsEmpty(const avl_t *tree)
{
	assert(NULL != tree);
	
	return (tree->root == NULL);
}



size_t AVLHeight(const avl_t *tree)
{
	assert(NULL != tree);
	
	if(AVLIsEmpty(tree))
	{
		return 0;
	}

	return tree->root->height;
}



size_t AVLCount(const avl_t *tree)
{
	assert(NULL != tree);
	
	return CountNodes(tree->root);
}



static size_t CountNodes(const node_t node)
{
	if(NULL == node)
	{
		return 0;
	}
	
	return 1 + CountNodes(node->LEFT) + CountNodes(node->RIGHT);
}



int AVLInsert(avl_t *tree, void *data)
{	
	node_t new_node = NULL;
	
	assert(NULL != tree);
	assert(NULL != tree->cmp_func);
	
	new_node = CreateNode(data);
	if(NULL == new_node)
	{
		return 1;
	}
	
	if(AVLIsEmpty(tree))
	{
		tree->root = new_node;
		return 0;
	}
	
	tree->root = AVLInsertNode(tree->root, new_node, tree->cmp_func);
	
	return 0;
}


static node_t AVLInsertNode(node_t node, node_t new_node, avl_cmp_func_t compare)
{
	int result_from_cmp_func = 0;
	int side = 0;
	
	if (node == NULL)
	{
		return new_node;
	}
	
	result_from_cmp_func = compare(node->data, new_node->data);
    side = WHERE_TO_GO(result_from_cmp_func);
    
	node->child[side] = AVLInsertNode(node->child[side], new_node, compare);

	return FixBalance(node);
}


void *AVLFind(const avl_t *tree, const void *data)
{
	node_t node_found = NULL;
	
	assert(NULL != tree);
	assert(NULL != tree->cmp_func);
	
	node_found = AVLFindNode(tree->root, data, tree->cmp_func);
	
	return (NULL == node_found) ? NULL : node_found->data;
}


static node_t AVLFindNode(node_t node, const void *data, avl_cmp_func_t compare)
{
	int result_from_cmp_func = 0;
	int right_or_left = 0;
	
	if(NULL == node)
	{
		return NULL;
	}
	
	result_from_cmp_func = compare(node->data, data);
	right_or_left = WHERE_TO_GO(result_from_cmp_func);
	
	if(NodeFound(result_from_cmp_func))
	{
		return node;
	}
	
	return AVLFindNode(node->child[right_or_left], data, compare);
}
	
	

int AVLForEach(avl_t *tree, avl_action_func_t action, void* params)
{
	assert(NULL != tree);
	assert(NULL != action);

	return AVLForEachNode(tree->root, action, params);
}

	
static int AVLForEachNode(node_t node, avl_action_func_t action, void* params)
{
	int status = 0;
	
	if(NULL == node)
	{
		return 0;
	}
	
	status = AVLForEachNode(node->LEFT, action, params);
	status = action(node->data, params);
	status = AVLForEachNode(node->RIGHT, action, params);
	
	return status;
}



void AVLRemove(avl_t *tree, void *data)
{
	assert(NULL != tree);
	assert(NULL != tree->cmp_func);
    
	tree->root = AVLRemoveNode(tree->root, data, tree->cmp_func);
}


static node_t AVLRemoveNode(node_t root, void *data, avl_cmp_func_t compare)
{
	int cmp_result = 0;
	int side = 0;
	
	if (root == NULL)
	{
		return NULL;
	}

	cmp_result = compare(root->data, data);
	side = WHERE_TO_GO(cmp_result);
	
	if(NodeFound(cmp_result))
	{
		if (HasOneOrNoChild(root))
		{
			node_t temp = root->LEFT ? root->LEFT : root->RIGHT;
            
			if (NodeIsLeaf(temp))
			{
				temp = root;
				root = NULL;
			}
			else
			{
				*root = *temp;
			}
            
			free(temp);
		}
		else
		{
			node_t temp = MinValueNode(root->RIGHT);
			root->data = temp->data;
			root->RIGHT = AVLRemoveNode(root->RIGHT, temp->data, compare);
		}
	}
	else
	{
		root->child[side] = AVLRemoveNode(root->child[side], data, compare);
	}

	if (NULL == root)
	{
		return NULL;
	}

	return FixBalance(root);
}
/***********************************  Helpers *******************************/
static node_t CreateNode(void *data)
{
	node_t new_node = (node_t)malloc(sizeof(struct avl_node));

	if(NULL == new_node)
	{
		return NULL;
	}
	
	new_node->data = data;
	new_node->RIGHT = NULL;
	new_node->LEFT = NULL;
	new_node->height = LEAF_HEIGHT;
	
	return new_node;
}	
	

static size_t NodeHeight(const node_t node)
{
	return (node == NULL) ? 0 : node->height;
}


static int GetBalance(node_t node)
{
	return (node == NULL) ? 0 : NodeHeight(node->LEFT) - NodeHeight(node->RIGHT);
}

	
static void UpdateHeight(node_t node)
{
	size_t left_height = 0;
	size_t right_height = 0;
	assert(NULL != node);
	
	left_height = NodeHeight(node->LEFT);
	right_height = NodeHeight(node->RIGHT);
	node->height = 1 + (MAX(left_height, right_height));
}


static int NodeFound(int cmp_result)
{
	return (0 == cmp_result);
}


static node_t Rotate(node_t root, int side)
{
	node_t pivot = root->child[!side]; 
	node_t pivot_child = pivot->child[side]; 
  
	pivot->child[side] = root; 
	root->child[!side] = pivot_child; 
  
	UpdateHeight(root);
	UpdateHeight(pivot);
  
	return pivot;
}


static int HasOneOrNoChild(node_t node)
{
	assert(NULL != node);
	
	return (NULL == node->LEFT || NULL == node->RIGHT);
}

static int NodeIsLeaf(node_t node)
{
	return (NULL == node);
}


static node_t FixBalance(node_t node)
{
	int balance_factor = 0;
	assert(NULL != node);
	
	UpdateHeight(node);
	balance_factor = GetBalance(node);
	
	/* RR case: */
	if(balance_factor < -1 &&  0 >= GetBalance(node->RIGHT))
	{
		return Rotate(node, ROTATE_LEFT);
	}
	
	/* RL case: */
	if(balance_factor < -1 && 1 == GetBalance(node->RIGHT))
	{
		node->RIGHT = Rotate(node->RIGHT, ROTATE_RIGHT);
		return Rotate(node, ROTATE_LEFT);
	}
	
	/* LL case: */
	if(balance_factor > 1 && 0 <= GetBalance(node->LEFT))
	{
		return Rotate(node, ROTATE_RIGHT);
	}
		
	/* LR case: */
	if(balance_factor > 1 && -1 == GetBalance(node->LEFT))
	{
		node->LEFT = Rotate(node->LEFT, ROTATE_LEFT);
		return Rotate(node, ROTATE_RIGHT);
	}
	
	return node;
}


static node_t MinValueNode(node_t node)
{
	node_t current = node;
    
    assert(NULL != node);
    
	while (current->LEFT != NULL)
	{
		current = current->LEFT;
	}

	return current;
}
