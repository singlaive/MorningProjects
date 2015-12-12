/*
 * beliketree.h
 *
 *  Created on: 2012-3-27
 *      Author: singlaive
 */

#ifndef BELIKETREE_H_
#define BELIKETREE_H_

typedef struct node
{
	char value;
	struct node *left_child;
	struct node *right_child;
}node_tag;

/*Make them macro? */
int BStree_element_add(uint8_t tree_handle, char element_value);
int BStree_element_delete(uint8_t tree_handle, char element_value);

#endif /* BELIKETREE_H_ */
