/*
 * linkit.h
 *
 *  Created on: 2011-12-8
 *      Author: singlaive
 */

#ifndef LINKIT_H_
#define LINKIT_H_

typedef enum
{
	false = 0,
	true
}bool;

typedef struct _list_node
{
	struct _list_node * prev;
	struct _list_node * next;
	int				   data;
}list_node;

typedef struct _list
{
	int num_nodes;
	list_node * head;
	list_node * tail;
}list;

list g_list;
list_node * g_current_elem;

int list_init();
int insert_head(int data);
int insert_tail(int data);
int insert_prev(int data);
int insert_next(int data);
int data_remove(int data);
int list_display();
int list_destroy();
int list_sort(bool is_ascending);
int test();
int display_help(char option);
int remove_elem(list_node * current);

#endif /* LINKIT_H_ */
