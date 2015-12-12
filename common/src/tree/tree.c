/*
 ============================================================================
 Name        : tree.c
 Author      : Murphy Meng
 Version     :
 Copyright   : Murphy Meng 2012
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "tree.h"

int32_t tree_create(uint8_t *p_tree_handle, int32_t (*compare)(void *key1, void *key2), void (*display)(void *data), void (*remove)(void *data));
void node_attach(tree_tag *bstree, bstree_node_tag *node_to_compare, bstree_node_tag *node_to_attach);
bstree_node_tag *retrieve_smallest(bstree_node_tag *node, bstree_node_tag **parent_node);
int32_t tree_traverse(uint8_t tree_handle, uint32_t traverse_method, void (*dump_node)(void *node));
void traverse_inorder(tree_tag *bstree, bstree_node_tag *current_node, void (*dump_node)(void *node));
void traverse_preorder(tree_tag *bstree, bstree_node_tag *current_node, void (*dump_node)(void *node));
void traverse_postorder(tree_tag *bstree, bstree_node_tag *current_node, void (*dump_node)(void *node));
bstree_node_tag *lookup(tree_tag *bstree, bstree_node_tag *current_node, void *data, bstree_node_tag **parent_node);
void destroy_all_nodes(tree_tag *bstree, bstree_node_tag *current_node);

void avl_node_attach(tree_tag *avltree, avltree_node_tag *node_to_compare, avltree_node_tag *node_to_attach);
void avl_rebalance(tree_tag *avltree, void *data);
uint32_t avl_node_height(avltree_node_tag *node, int32_t *balance_factor);
avltree_node_tag *avl_rotation_LL(avltree_node_tag *pivot_node);
avltree_node_tag *avl_rotation_RR(avltree_node_tag *pivot_node);
avltree_node_tag *avl_rotation_LR(avltree_node_tag *pivot_node);
avltree_node_tag *avl_rotation_RL(avltree_node_tag *pivot_node);
avltree_node_tag *avl_rebalance_ex(avltree_node_tag *node);
avltree_node_tag *avltree_remove(tree_tag *avltree, avltree_node_tag *node, void *p_data);

rbtree_node_tag *rb_node_attach(tree_tag *rbtree, rbtree_node_tag *node_to_compare, rbtree_node_tag *node_to_attach);
rbtree_node_tag *rb_rebalance(rbtree_node_tag *node, int nav_dir);
rbtree_node_tag *rb_rotation_LL(rbtree_node_tag *current_root);
rbtree_node_tag *rb_rotation_RR(rbtree_node_tag *current_root);
rbtree_node_tag *rb_rotation_LR(rbtree_node_tag *current_root);
rbtree_node_tag *rb_rotation_RL(rbtree_node_tag *current_root);
bool is_red(rbtree_node_tag *node);
void rb_dump_node(void *node);
rbtree_node_tag *tree_remove(tree_tag *rbtree, rbtree_node_tag *node, void *p_data, bool *p_balanced);
rbtree_node_tag *rebalance(rbtree_node_tag *node, int compare_result, bool *p_balanced);
uint32_t rb_sanity_check(tree_tag *rbtree, rbtree_node_tag *node);

int32_t BStree_create(uint8_t *p_tree_handle,
					  int32_t (*compare)(void *key1, void *key2),
					  void (*display)(void *data),
					  void (*remove)(void *data))
{
	int32_t status = EXIT_FAILURE;

	status = tree_create(p_tree_handle, compare, display, remove);

	return status;
}

int32_t tree_create(uint8_t *p_tree_handle,
	 	  		    int32_t (*compare)(void *key1, void *key2),
				    void (*display)(void *data),
				    void (*remove)(void *data))
{
	int32_t status = EXIT_FAILURE;
	tree_tag *tree;

	tree = malloc(sizeof(tree_tag));
	if (NULL != tree)
	{
		(void)COMMON_handle_create((void *)tree, p_tree_handle);
		tree->num_nodes = 0U;
		tree->root = NULL;
		tree->compare = compare;
		tree->display = display;
		tree->remove = remove;
		status = EXIT_SUCCESS;
	}
	else
	{
		printf("Error - malloc failed.\n");
	}

	return status;
}

int32_t BStree_attach(uint8_t tree_handle, void *data)
{
	int32_t status = EXIT_FAILURE;
	tree_tag *bstree;
	bstree_node_tag *new_node = NULL;

	(void)COMMON_object_retrieve(tree_handle, (void **)&bstree);

	new_node = malloc(sizeof(bstree_node_tag));
	if (NULL != new_node)
	{
		memset(new_node, 0x00, sizeof(bstree_node_tag));
		new_node->data = data;

		if (NULL != bstree->root)
		{
			node_attach(bstree, bstree->root, new_node);
		}
		else
		{
			bstree->root = new_node;
		}

		status = EXIT_SUCCESS;
		bstree->num_nodes++;
	}
	else
	{
		printf("Error - malloc failed.\n");
	}

	return status;
}

void node_attach(tree_tag *bstree, bstree_node_tag *node_to_compare, bstree_node_tag *node_to_attach)
{
	int32_t compare_result;

	compare_result = bstree->compare(node_to_attach->data, node_to_compare->data);
	if (compare_result < 0)
	{
		if (NULL != node_to_compare->left_child)
		{
			node_attach(bstree, node_to_compare->left_child, node_to_attach);
		}
		else
		{
			node_to_compare->left_child = node_to_attach;
		}
	}
	else
	{
		if (NULL != node_to_compare->right_child)
		{
			node_attach(bstree, node_to_compare->right_child, node_to_attach);
		}
		else
		{
			node_to_compare->right_child = node_to_attach;
		}
	}
}

int32_t BStree_remove(uint8_t tree_handle, void *data)
{
	int32_t status = EXIT_FAILURE;
	tree_tag *bstree;
	bstree_node_tag *node_found, *parent_node, **position_to_fill, *candidate_node;

	(void)COMMON_object_retrieve(tree_handle, (void **)&bstree);

	if (NULL != bstree->root)
	{
		parent_node = NULL;
		node_found = lookup(bstree, bstree->root, data, &parent_node);
		if (NULL != node_found)
		{
			if (NULL != bstree->remove)
			{
				bstree->remove(node_found->data);
			}

			if (NULL == parent_node)
			{
				position_to_fill = (bstree_node_tag **)&bstree->root;
			}
			else if (parent_node->left_child == node_found)
			{
				position_to_fill = &parent_node->left_child;
			}
			else
			{
				position_to_fill = &parent_node->right_child;
			}

			if (NULL != node_found->right_child)
			{
				candidate_node = retrieve_smallest(node_found->right_child, &parent_node);
				*position_to_fill = candidate_node;

				/* No matter candidate_node->right_child is NULL or not, the following is whatever we need to do. :) */
				parent_node->left_child = candidate_node->right_child;

				/* Rebuild the children of the node to remove. */
				candidate_node->left_child = node_found->left_child;
				if (candidate_node->right_child != node_found->right_child)
					candidate_node->right_child = node_found->right_child;
			}
			else
			{
				/* Simply attach the only left child to the parent of the node to remove. */
				*position_to_fill = node_found->left_child;
			}

			free(node_found);
			bstree->num_nodes--;

			status = EXIT_SUCCESS;
		}
		else
		{
			printf("Warn - Data to remove not exist.");
		}
	}

	return status;
}

bstree_node_tag *retrieve_smallest(bstree_node_tag *node, bstree_node_tag **parent_node)
{
	bstree_node_tag *node_found = NULL;

	if (NULL != node->left_child)
	{
		*parent_node = node;
		node_found = retrieve_smallest(node->left_child, parent_node);
	}
	else
	{
		node_found = node;
	}

	return node_found;
}

int32_t BStree_traverse(uint8_t tree_handle, uint32_t traverse_method)
{
	int32_t status = EXIT_SUCCESS;

	status = tree_traverse(tree_handle, traverse_method, NULL);

	return status;
}

int32_t tree_traverse(uint8_t tree_handle, uint32_t traverse_method, void (*dump_node)(void *node))
{
	int32_t status = EXIT_SUCCESS;
	tree_tag *bstree;

	(void)COMMON_object_retrieve(tree_handle, (void **)&bstree);

	if (NULL != bstree->root)
	{
		switch (traverse_method)
		{
			case TRAVERSE_METHOD_INORDER:
				traverse_inorder(bstree, bstree->root, dump_node);
				break;
			case TRAVERSE_METHOD_PREORDER:
				traverse_preorder(bstree, bstree->root, dump_node);
				break;
			case TRAVERSE_METHOD_POSTORDER:
				traverse_postorder(bstree, bstree->root, dump_node);
				break;
			default:
				printf("Error - Unsupported parameters.\n");
				status = EXIT_FAILURE;
				break;
		}
	}
	else
	{
		printf("Warn - Empty tree.\n");
	}

	return status;
}
void traverse_inorder(tree_tag *bstree, bstree_node_tag *current_node, void (*dump_node)(void *node))
{
	if (NULL != current_node->left_child)
	{
		traverse_inorder(bstree, current_node->left_child, dump_node);
	}

	bstree->display(current_node->data);
	if (NULL != dump_node)
	{
		dump_node(current_node);
	}

	if (NULL != current_node->right_child)
	{
		traverse_inorder(bstree, current_node->right_child, dump_node);
	}
}

void traverse_preorder(tree_tag *bstree, bstree_node_tag *current_node, void (*dump_node)(void *node))
{
	bstree->display(current_node->data);
	if (NULL != dump_node)
	{
		dump_node(current_node);
	}

	if (NULL != current_node->left_child)
	{
		traverse_preorder(bstree, current_node->left_child, dump_node);
	}

	if (NULL != current_node->right_child)
	{
		traverse_preorder(bstree, current_node->right_child, dump_node);
	}

}

void traverse_postorder(tree_tag *bstree, bstree_node_tag *current_node, void (*dump_node)(void *node))
{
	if (NULL != current_node->left_child)
	{
		traverse_postorder(bstree, current_node->left_child, dump_node);
	}

	if (NULL != current_node->right_child)
	{
		traverse_postorder(bstree, current_node->right_child, dump_node);
	}

	bstree->display(current_node->data);
	if (NULL != dump_node)
	{
		dump_node(current_node);
	}
}

int32_t BStree_lookup(uint8_t tree_handle, void *data, bool *p_data_found)
{
	tree_tag *bstree;
	bstree_node_tag *parent_node;
	*p_data_found = false;

	(void)COMMON_object_retrieve(tree_handle, (void **)&bstree);

	if (NULL != bstree->root)
	{
		if (NULL != lookup(bstree, bstree->root, data, &parent_node))
		{
			*p_data_found = true;
		}
	}

	return EXIT_SUCCESS;
}

bstree_node_tag *lookup(tree_tag *bstree, bstree_node_tag *current_node, void *data, bstree_node_tag **parent_node)
{
	int32_t result;
	bstree_node_tag *node_found;

	result = bstree->compare(data, current_node->data);
	if (result < 0)
	{
		if (NULL != current_node->left_child)
		{
			*parent_node = current_node;
			node_found = lookup(bstree, current_node->left_child, data, parent_node);
		}
		else
		{
			node_found = NULL;
		}
	}
	else if (result > 0)
	{
		if (NULL != current_node->right_child)
		{
			*parent_node = current_node;
			node_found = lookup(bstree, current_node->right_child, data, parent_node);
		}
		else
		{
			node_found = NULL;
		}
	}
	else
	{
		node_found = current_node;
	}

	return node_found;
}

#if 0
int32_t BStree_dump(uint8_t tree_handle)
{
	int32_t status = EXIT_FAILURE;
	tree_tag *bstree;

	(void)COMMON_object_retrieve(tree_handle, (void **)&bstree);

	if (NULL != bstree->root)
	{
	}
	return EXIT_SUCCEED;
}
#endif
uint32_t BStree_size(uint8_t tree_handle)
{
	tree_tag *bstree;

	(void)COMMON_object_retrieve(tree_handle, (void **)&bstree);
	return bstree->num_nodes;
}

int32_t BStree_destroy(uint8_t tree_handle)
{
	int32_t status = EXIT_FAILURE;
	tree_tag *bstree;

	(void)COMMON_object_retrieve(tree_handle, (void **)&bstree);

	if (NULL != bstree->root)
	{
		destroy_all_nodes(bstree, bstree->root);
		memset(bstree, 0x00, sizeof(tree_tag));
#if 0
		if (0 != COMMON_handle_destroy(tree_handle))
		{
			printf("Error - COMMON_handle_destroy failed.\n");
		}
#endif
		status = EXIT_SUCCESS;
	}
	else
	{
		printf("Warn - Tree empty.\n");
	}

	return status;
}

void destroy_all_nodes(tree_tag *bstree, bstree_node_tag *current_node)
{
	if (NULL != current_node->left_child)
	{
		destroy_all_nodes(bstree, current_node->left_child);
	}

	if (NULL != current_node->right_child)
	{
		destroy_all_nodes(bstree, current_node->right_child);
	}

	if (NULL != bstree->remove)
	{
		bstree->remove(current_node->data);
	}

	free(current_node);
}

/* ================================================================================ */
/* AVL tree*/
/* ================================================================================ */

int32_t AVLtree_create(uint8_t *p_tree_handle,
				  int32_t (*compare)(void *key1, void *key2),
				  void (*display)(void *data),
				  void (*remove)(void *data))
{
	int32_t status = EXIT_FAILURE;

	status = tree_create(p_tree_handle, compare, display, remove);

	return status;
}

int32_t AVLtree_attach(uint8_t tree_handle, void *data)
{
	int32_t status = EXIT_FAILURE;
	tree_tag *avltree;
	avltree_node_tag *new_node = NULL;

	(void)COMMON_object_retrieve(tree_handle, (void **)&avltree);

	new_node = malloc(sizeof(avltree_node_tag));
	if (NULL != new_node)
	{
		memset(new_node, 0x00, sizeof(avltree_node_tag));
		new_node->data = data;

		if (NULL != avltree->root)
		{
			avl_node_attach(avltree, (avltree_node_tag *)avltree->root, new_node);
			avl_rebalance(avltree, data);
		}
		else
		{
			avltree->root = new_node;
		}

		status = EXIT_SUCCESS;
		avltree->num_nodes++;
	}
	else
	{
		printf("Error - malloc failed.\n");
	}

	return status;
}

void avl_node_attach(tree_tag *avltree, avltree_node_tag *node_to_compare, avltree_node_tag *node_to_attach)
{
	int32_t compare_result;

	compare_result = avltree->compare(node_to_attach->data, node_to_compare->data);
	if (compare_result < 0)
	{
		if (NULL != node_to_compare->left_child)
		{
			avl_node_attach(avltree, node_to_compare->left_child, node_to_attach);
		}
		else
		{
			node_to_compare->left_child = node_to_attach;
		}
	}
	else
	{
		if (NULL != node_to_compare->right_child)
		{
			avl_node_attach(avltree, node_to_compare->right_child, node_to_attach);
		}
		else
		{
			node_to_compare->right_child = node_to_attach;
		}
	}
}

void avl_rebalance(tree_tag *avltree, void *data)
{
	avltree_node_tag **current_node = (avltree_node_tag **)&(avltree->root);
	avltree_node_tag **pivot_node = NULL;
	int32_t balance_factor = 0U;

	while (NULL != *current_node)
	{
		(void)avl_node_height(*current_node, &balance_factor);
		if (abs(balance_factor) > 1)
		{
			pivot_node = current_node;
		}

		if (avltree->compare(data, (*current_node)->data) < 0)
		{

			current_node = &(*current_node)->left_child;
		}
		else
		{
			current_node = &(*current_node)->right_child;
		}
	}

	if (NULL != pivot_node)
	{
		if (avltree->compare(data, (*pivot_node)->data) < 0)
		{
			if (avltree->compare(data, (*pivot_node)->left_child->data) < 0)
			{
				*pivot_node = avl_rotation_LL(*pivot_node);
			}
			else
			{
				*pivot_node = avl_rotation_LR(*pivot_node);
			}
		}
		else
		{
			if (avltree->compare(data, (*pivot_node)->right_child->data) < 0)
			{
				*pivot_node = avl_rotation_RL(*pivot_node);
			}
			else
			{
				*pivot_node = avl_rotation_RR(*pivot_node);
			}
		}
	}
}

uint32_t avl_node_height(avltree_node_tag *node, int32_t *balance_factor)
{
	uint32_t height_L = 0;
	uint32_t height_R = 0;
	uint32_t result;

	if (NULL != node->left_child)
	{
		height_L = avl_node_height(node->left_child, NULL);
	}

	if (NULL != node->right_child)
	{
		height_R = avl_node_height(node->right_child, NULL);
	}

	if (NULL != balance_factor)
	{
		*balance_factor = height_L - height_R;
	}

	if (height_L > height_R)
	{
		result = height_L;
	}
	else
	{
		result = height_R;
	}

	return (result+1);
}

avltree_node_tag *avl_rotation_LL(avltree_node_tag *node)
{
	avltree_node_tag *temp;

	temp = node->left_child;
	node->left_child = temp->right_child;
	temp->right_child = node;

	return temp;
}

avltree_node_tag *avl_rotation_RR(avltree_node_tag *node)
{
	avltree_node_tag *temp;

	temp = node->right_child;
	node->right_child = temp->left_child;
	temp->left_child = node;
	return temp;
}

avltree_node_tag * avl_rotation_LR(avltree_node_tag *node)
{
	node->left_child = avl_rotation_RR(node->left_child);
	return avl_rotation_LL(node);
}

avltree_node_tag * avl_rotation_RL(avltree_node_tag *node)
{
	node->right_child = avl_rotation_LL(node->right_child);
	return avl_rotation_RR(node);
}

int32_t AVLtree_remove(uint8_t tree_handle, void *data)
{
	int32_t status = EXIT_SUCCESS;
	tree_tag *avltree;

	(void)COMMON_object_retrieve(tree_handle, (void **)&avltree);
	avltree->root = (void *)avltree_remove(avltree, avltree->root, data);

	return status;
}

avltree_node_tag *avltree_remove(tree_tag *avltree, avltree_node_tag *node, void *p_data)
{
	int8_t compare_result;
	if (NULL == node)
	{
		printf("Info - No matched data.\n");
	}
	else
	{
		compare_result = avltree->compare(p_data, node->data);
	    if (0 == compare_result)
	    {
	    	avltree_node_tag *temp = NULL;

	        if (NULL == node->left_child || NULL == node->right_child)
	        {
	     	    temp = (node->left_child == NULL) ? node->right_child : node->left_child;

	    	    /* if p_data points to the same address of node->data, dont remove it. This means there is
	    	     * an internal node is pointing to this data, which had been done below. */
	    	    if ((p_data != node->data) && (NULL != avltree->remove))
	    	    {
	    	    	avltree->remove(node->data);
	    	    }

				avltree->num_nodes--;
	    	    free(node);
	    	    node = temp;
	        }
	        else
	        {
				temp = node->left_child;

				while (NULL != temp->right_child)
				{
					temp = temp->right_child;
				}

				/* The actual removal of the data. */
				if (NULL != avltree->remove)
				{
					avltree->remove(node->data);
				}
	    	    /* Save the data of the current node as its predecessor. */
				node->data = temp->data;
				p_data = temp->data;
	        }
	    }

	    /* Compare again in case the data of current node has changed. */
	    if (NULL != node)
	    {
			compare_result = avltree->compare(p_data, node->data);
			if (compare_result <= 0)
			{
				node->left_child = avltree_remove(avltree, node->left_child, p_data);
			}
			else
			{
				node->right_child = avltree_remove(avltree, node->right_child, p_data);
			}

			node = avl_rebalance_ex(node);
	    }
	}

	return node;
}

avltree_node_tag *avl_rebalance_ex(avltree_node_tag *node)
{
	int32_t balance_factor = 0;

	(void)avl_node_height(node, &balance_factor);

	/* balance_factor > 0 means it is 2, otherewise it is -2. For the first case, it is
	 * left heavy and the other way round for the other case. */
	if (balance_factor > 1)
	{
		(void)avl_node_height(node->left_child, &balance_factor);
		if (balance_factor > 0) /* Again, left heavy. */
		{
			node = avl_rotation_LL(node);
		}
		else if (balance_factor < 0)
		{
			node = avl_rotation_LR(node);
		}
		else
		{
			/* Do nothing. */
		}
	}
	else if (balance_factor < -1)
	{
		(void)avl_node_height(node->right_child, &balance_factor);
		if (balance_factor < 0) /* Right heavy. */
		{
			node = avl_rotation_RR(node);
		}
		else if (balance_factor > 0)
		{
			node = avl_rotation_RL(node);
		}
		else
		{
			/* Do nothing. */
		}
	}
	else
	{
		/* Do nothing. */
	}

	return node;
}




int32_t RBtree_create(uint8_t *p_tree_handle,
					  int32_t (*compare)(void *key1, void *key2),
					  void (*display)(void *data),
					  void (*remove)(void *data))
{
	int32_t status = EXIT_FAILURE;

	status = tree_create(p_tree_handle, compare, display, remove);

	return status;
}

int32_t RBtree_attach(uint8_t tree_handle, void *data)
{
	int32_t status = EXIT_FAILURE;
	tree_tag *rbtree;
	rbtree_node_tag *new_node = NULL;
	(void)COMMON_object_retrieve(tree_handle, (void **)&rbtree);

	new_node = malloc(sizeof(tree_tag));
	if (NULL != new_node)
	{
		memset(new_node, 0x00, sizeof(avltree_node_tag));
		new_node->data = data;

		if (NULL != rbtree->root)
		{
			rbtree->root = rb_node_attach(rbtree, rbtree->root, new_node);
		}
		else
		{
			rbtree->root = new_node;
		}
		((rbtree_node_tag *)rbtree->root)->color = black;

		status = EXIT_SUCCESS;
		rbtree->num_nodes++;
	}
	else
	{
		printf("Error - malloc failed.\n");
	}

	return status;
}

rbtree_node_tag *rb_node_attach(tree_tag *rbtree, rbtree_node_tag *node_to_compare, rbtree_node_tag *node_to_attach)
{
	int32_t compare_result;

	compare_result = rbtree->compare(node_to_attach->data, node_to_compare->data);
	if (0 != compare_result)
	{
		if (compare_result < 0)
		{
			if (NULL != node_to_compare->left_child)
			{
				node_to_compare->left_child = rb_node_attach(rbtree, node_to_compare->left_child, node_to_attach);
			}
			else
			{
				node_to_compare->left_child = node_to_attach;
			}
		}
		else
		{
			if (NULL != node_to_compare->right_child)
			{
				node_to_compare->right_child = rb_node_attach(rbtree, node_to_compare->right_child, node_to_attach);
			}
			else
			{
				node_to_compare->right_child = node_to_attach;
			}
		}

		node_to_compare = rb_rebalance(node_to_compare, compare_result);
	}

	return node_to_compare;
}

rbtree_node_tag *rb_rebalance(rbtree_node_tag *node, int nav_dir)
{
	if (nav_dir < 0)
	{
		if (is_red(node->left_child))
		{
			if (is_red(node->right_child))
			{
				/* Case I: flip the colors of relevant nodes. */
				node->color = red;
				node->left_child->color = black;
				node->right_child->color = black;
			}
			else
			{
				if (is_red(node->left_child->left_child))
				{
					/* Case II: */
					node = rb_rotation_LL(node);
				}
				else if (is_red(node->left_child->right_child))
				{
					/* Case III */
					node = rb_rotation_LR(node);
				}
			}
		}
	}
	else
	{
		if (is_red(node->right_child))
		{
			if (is_red(node->left_child))
			{
				/* Case I: flip the colors of relevant nodes. */
				node->color = red;
				node->left_child->color = black;
				node->right_child->color = black;
			}
			else
			{
				if (is_red(node->right_child->right_child))
				{
					/* Case II: */
					node = rb_rotation_RR(node);
				}
				else if (is_red(node->right_child->left_child))
				{
					/* Case III: */
					node = rb_rotation_RL(node);
				}
			}
		}
	}

	return node;
}

rbtree_node_tag *rb_rotation_LL(rbtree_node_tag *current_root)
{
	rbtree_node_tag *new_root;

	new_root = current_root->left_child;
	current_root->left_child = new_root->right_child;
	new_root->right_child = current_root;

	current_root->color = red;
	new_root->color = black;

	return new_root;
}

rbtree_node_tag *rb_rotation_RR(rbtree_node_tag *current_root)
{
	rbtree_node_tag *new_root;

	new_root = current_root->right_child;
	current_root->right_child = new_root->left_child;
	new_root->left_child = current_root;

	current_root->color = red;
	new_root->color = black;

	return new_root;
}

rbtree_node_tag *rb_rotation_LR(rbtree_node_tag *current_root)
{
	current_root->left_child = rb_rotation_RR(current_root->left_child);
	return rb_rotation_LL(current_root);
}

rbtree_node_tag *rb_rotation_RL(rbtree_node_tag *current_root)
{
	current_root->right_child = rb_rotation_LL(current_root->right_child);
	return rb_rotation_RR(current_root);
}

bool is_red(rbtree_node_tag *node)
{
	return node != NULL && node->color == red;
}

int32_t RBtree_traverse(uint8_t tree_handle, uint32_t traverse_method)
{
	int32_t status = EXIT_SUCCESS;

	status = tree_traverse(tree_handle, traverse_method, rb_dump_node);

	return status;
}

void rb_dump_node(void *node)
{
	printf("---------color %s\n", (((rbtree_node_tag *)node)->color == red ) ? "red" : "black");
}

int32_t RBtree_remove(uint8_t tree_handle, void *data)
{
	int32_t status = EXIT_FAILURE;
	tree_tag *rbtree;
	bool balanced = false;

	(void)COMMON_object_retrieve(tree_handle, (void **)&rbtree);

	if (NULL != rbtree->root)
	{
		rbtree->root = tree_remove(rbtree, rbtree->root, data, &balanced);
		if (NULL != rbtree->root)
		{
			((rbtree_node_tag *)rbtree->root)->color = black;
		}

		rbtree->num_nodes--;

		status = EXIT_SUCCESS;
	}
	else
	{
		printf("Error - Empty tree.");
	}

	return status;
}

rbtree_node_tag *tree_remove(tree_tag *rbtree, rbtree_node_tag *node, void *p_data, bool *p_balanced)
{
	int8_t compare_result;

	if (NULL == node)
	{
	   *p_balanced = true;
	   printf("Info - No matched data.\n");
	}
	else
	{
		compare_result = rbtree->compare(p_data, node->data);
	    if (0 == compare_result)
	    {
	    	rbtree_node_tag *temp = NULL;

	        if (NULL == node->left_child || NULL == node->right_child)
	        {
	     	    temp = (node->left_child == NULL) ? node->right_child : node->left_child;

	    	    /* Case 0 */
	    	    if (is_red(node))
	    	    {
	    		    *p_balanced = true;
	    	    }
	    	    else if (is_red(temp))
	    	    {
	    	 	    temp->color = black;
	    		    *p_balanced = true;
	    	    }

	    	    /* if p_data points to the same address of node->data, dont remove it. This means there is
	    	     * an internal node is pointing to this data, which had been done below. */
	    	    if ((p_data != node->data) && (NULL != rbtree->remove))
	    	    {
	    	    	rbtree->remove(node->data);
	    	    }
if (p_data == node->data)
{printf("mark！\n");}
	    	    free(node);
	    	    node = temp;
	        }
	        else
	        {
				temp = node->left_child;

				while (NULL != temp->right_child)
				{
					temp = temp->right_child;
				}

				/* The actual removal of the data. */
				if (NULL != rbtree->remove)
				{
					rbtree->remove(node->data);
				}
	    	    /* Save the data of the current node as its predecessor. */
				node->data = temp->data;
				p_data = temp->data;
	        }
	    }

	    /* Compare again in case the data of current node has changed. */
	    if (NULL != node)
	    {
			compare_result = rbtree->compare(p_data, node->data);
			if (compare_result <= 0)
			{
				node->left_child = tree_remove(rbtree, node->left_child, p_data, p_balanced);
			}
			else
			{
				node->right_child = tree_remove(rbtree, node->right_child, p_data, p_balanced);
			}

			if (false == *p_balanced)
			{
			   node = rebalance(node, compare_result, p_balanced);
			}
	    }
	}

	return node;
}

rbtree_node_tag *rebalance(rbtree_node_tag *node, int compare_result, bool *p_balanced)
{
	rbtree_node_tag *temp = node;
	rbtree_node_tag *subtree = (compare_result <= 0) ? node->right_child : node->left_child;

	/* Case reduction, remove red sibling */
	if (is_red(subtree))
	{
		if (compare_result < 0)
		{
			node = rb_rotation_LL(node);
		}
		else
		{
			node = rb_rotation_RR(node);
		}

		subtree = (compare_result < 0) ? temp->right_child : node->left_child;
	}

	if (NULL != subtree)
	{
		if (!is_red(subtree->left_child) && !is_red(subtree->right_child))
		{
			if ( is_red(temp))
			{
				*p_balanced = true;
			}

			temp->color = black;
			subtree->color = red;
		}
		else
		{
			enum {red, black} color = temp->color;
			int node_changed = (node != temp);

			if (is_red(subtree->right_child))
			{
				temp = rb_rotation_RR(temp);
			}
			else
			{
				temp = rb_rotation_RL(temp);
			}

			temp->color = color;
			temp->left_child->color = black;
			temp->right_child->color = black;

			if (false == node_changed)
			{
				node = temp;
			}
			else
			{
				if (compare_result < 0)
				{
					node->left_child = temp;
				}
				else
				{
					node->right_child = temp;
				}
			}
			*p_balanced = true;
		}
	}

	return node;
}

int32_t RBtree_sanity_check(uint8_t tree_handle)
{
	int32_t status = EXIT_SUCCESS;
	tree_tag *rbtree;

	(void)COMMON_object_retrieve(tree_handle, (void **)&rbtree);
	(void)rb_sanity_check(rbtree, (rbtree_node_tag *)rbtree->root);

	return status;
}

uint32_t rb_sanity_check(tree_tag *rbtree, rbtree_node_tag *node)
{
	uint32_t height = 0U;
	uint32_t height_L = 0U;
	uint32_t height_R = 0U;

	if (NULL == node)
	{
		height = 1U;
	}
	else
	{
		if (is_red(node))
		{
			if ((is_red(node->right_child)) || (is_red(node->right_child)))
			{
				printf("Warn - Red violation by node ");
				rbtree->display(node->data);
			}
		}

		height_L = rb_sanity_check(rbtree, node->left_child);
		height_R = rb_sanity_check(rbtree, node->right_child);

		if ((0 != height_L) && (0 != height_R))
		{
			if (height_L != height_R)
			{
				printf("Warn - Black violation by node ");
				rbtree->display(node->data);
			}
			else
			{
				height = is_red(node) ? height_L : (height_L + 1);
			}
		}
	}

	return height;
}






