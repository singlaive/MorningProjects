/*
 * tree.h
 *
 *  Created on: 2012-3-26
 *      Author: singlaive
 */

#ifndef TREE_H_
#define TREE_H_

#include "common.h"

#define TRAVERSE_METHOD_INORDER   1U
#define TRAVERSE_METHOD_PREORDER  2U
#define TRAVERSE_METHOD_POSTORDER 3U

typedef struct bstree
{
	uint32_t num_nodes; /* Size of the tree. */
	void *root;
	int32_t (*compare)(void *key1, void *key2);
	void (*display)(void *data);
	void (*remove)(void *data);
}tree_tag;

/* ================================================================================ */
/* Binary search tree*/
/* ================================================================================ */

typedef struct bstree_node
{
	void *data;
	struct bstree_node *left_child;
	struct bstree_node *right_child;
}bstree_node_tag;

/* Create a binary search tree. */
int32_t BStree_create(uint8_t *p_tree_handle,
				  int32_t (*compare)(void *key1, void *key2),
				  void (*display)(void *data),
				  void (*remove)(void *data));

/* Attach a user defined node to an existing tree specified by its handle. */
int32_t BStree_attach(uint8_t tree_handle, void *data);
int32_t BStree_remove(uint8_t tree_handle, void *data);
int32_t BStree_lookup(uint8_t tree_handle, void *data, bool *p_data_found);
int32_t BStree_traverse(uint8_t tree_handle, uint32_t traverse_method);
int32_t BStree_dump(uint8_t tree_handle);
uint32_t BStree_size(uint8_t tree);
int32_t BStree_destroy(uint8_t tree_handle);

/* ================================================================================ */
/* AVL tree*/
/* ================================================================================ */

typedef struct avltree_node
{
	void *data;
	struct avltree_node *left_child;
	struct avltree_node *right_child;
}avltree_node_tag;

/* Create a binary search tree. */
int32_t AVLtree_create(uint8_t *p_tree_handle,
				  int32_t (*compare)(void *key1, void *key2),
				  void (*display)(void *data),
				  void (*remove)(void *data));

/* Attach a user defined node to an existing tree specified by its handle. */
int32_t AVLtree_attach(uint8_t tree_handle, void *data);
int32_t AVLtree_remove(uint8_t tree_handle, void *data);
int32_t AVLtree_lookup(uint8_t tree_handle, void *data, bool *p_data_found);
int32_t AVLtree_traverse(uint8_t tree_handle, uint32_t traverse_method);
int32_t AVLtree_dump(uint8_t tree_handle);
uint32_t AVLtree_size(uint8_t tree);
int32_t AVLtree_destroy(uint8_t tree_handle);

/* ================================================================================ */
/* RB tree*/
/* ================================================================================ */

typedef struct rbtree_node
{
	void *data;
	struct rbtree_node *left_child;
	struct rbtree_node *right_child;
	enum {red, black} color;
}rbtree_node_tag;

/* Create a binary search tree. */
int32_t RBtree_create(uint8_t *p_tree_handle,
					  int32_t (*compare)(void *key1, void *key2),
					  void (*display)(void *data),
					  void (*remove)(void *data));

int32_t RBtree_attach(uint8_t tree_handle, void *data);
int32_t RBtree_remove(uint8_t tree_handle, void *data);
int32_t RBtree_lookup(uint8_t tree_handle, void *data, bool *p_data_found);
int32_t RBtree_traverse(uint8_t tree_handle, uint32_t traverse_method);
int32_t RBtree_dump(uint8_t tree_handle);
uint32_t RBtree_size(uint8_t tree);
int32_t RBtree_destroy(uint8_t tree_handle);
int32_t RBtree_sanity_check(uint8_t tree_handle);










#endif /* TREE_H_ */
