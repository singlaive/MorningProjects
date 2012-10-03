/*
 ============================================================================
 Name        : Linked.c
 Author      : Murphy Meng
 Version     :
 Copyright   : Free
 Description : Linked list exericise, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkit.h"

int list_init(void)
{
	memset((void *)&g_list, 0x00, sizeof(g_list));
	g_current_elem = NULL;
	return EXIT_SUCCESS;
}


int insert_next(int data)
{
	list_node * new_node = NULL;

	if ((NULL == g_current_elem) && (0 != g_list.num_nodes))
	{
		printf("Invalid current element.\n");
		return EXIT_FAILURE;
	}
	else
	{
		new_node = (list_node *)malloc(sizeof(list_node));
		if (NULL == new_node)
		{
			return EXIT_FAILURE;
		}

		new_node->data = data;
		if (0 == g_list.num_nodes)
		{
			printf("Empty list.\n");
			new_node->next = NULL;
			new_node->prev = NULL;
			g_list.tail = new_node;
			g_list.head = new_node;
			g_current_elem = g_list.head;
		}
		else
		{
			new_node->prev = g_current_elem;
			new_node->next = g_current_elem->next;
			g_current_elem->next = new_node;

			if (g_current_elem == g_list.tail)
			{
				g_list.tail = new_node;
			}
			else
			{
				new_node->next->prev = new_node;
			}
		}
	}

	return EXIT_SUCCESS;
}

int insert_prev(int data)
{
	list_node * new_node = NULL;

	if ((NULL == g_current_elem) && (0 != g_list.num_nodes))
	{
		printf("Invalid current element.\n");
		return EXIT_FAILURE;
	}
	else
	{
		new_node = (list_node *)malloc(sizeof(list_node));
		if (NULL == new_node)
		{
			return EXIT_FAILURE;
		}

		new_node->data = data;
		if (0 == g_list.num_nodes)
		{
			printf("Empty list.\n");
			new_node->next = NULL;
			new_node->prev = NULL;
			g_list.tail = new_node;
			g_list.head = new_node;
			g_current_elem = g_list.head;
		}
		else
		{
			new_node->next = g_current_elem;
			new_node->prev = g_current_elem->prev;
			g_current_elem->prev = new_node;

			if (g_current_elem == g_list.head)
			{
				g_list.head = new_node;
			}
			else
			{
				new_node->prev->next = new_node;
			}
		}
	}

	return EXIT_SUCCESS;
}


int insert_head(int data)
{
	list_node * new_node;

	new_node = (list_node *)malloc(sizeof(list_node));
	if (NULL == new_node)
	{
		return EXIT_FAILURE;
	}

	new_node->data = data;
	if (0 == g_list.num_nodes)
	{
		new_node->next = NULL;
		g_list.tail = new_node;
		g_current_elem = new_node;
	}
	else
	{
		new_node->next = g_list.head;
		new_node->next->prev = new_node;
	}

	new_node->prev = NULL;
	g_list.head = new_node;
	++g_list.num_nodes;
	return EXIT_SUCCESS;
}

int insert_tail(int data)
{
	list_node * new_node;

	new_node = (list_node *)malloc(sizeof(list_node));
	if (NULL == new_node)
	{
		return EXIT_FAILURE;
	}

	new_node->data = data;
	if (0 == g_list.num_nodes)
	{
		new_node->prev = NULL;
		g_list.head = new_node;
		g_current_elem = new_node;
	}
	else
	{
		new_node->prev = g_list.tail;
		new_node->prev->next = new_node;
	}

	new_node->next = NULL;
	g_list.tail = new_node;
	++g_list.num_nodes;
	return EXIT_SUCCESS;
}

int data_remove(int data)
{
	list_node * current = NULL;
	bool is_found = false;

	current = g_list.head;
	if (NULL == current)
	{
		printf("Error - empty list.\n");
		return EXIT_FAILURE;
	}

	do
	{
		if (data == current->data)
		{
			is_found = true;
			break;
		}

		current = current->next;
	}while (NULL != current);

	if (false == is_found)
	{
		printf("Error - Data not found.\n");
		return EXIT_SUCCESS;
	}

	if (current == g_current_elem)
	{
		g_current_elem = g_current_elem->next;
	}

	if (current == g_list.head)
	{
		g_list.head = g_list.head->next;
		if (NULL == g_list.head)
		{
			g_list.tail = NULL;
		}
		else
		{
			g_list.head->prev = NULL;
		}
	}
	else
	{
		current->prev->next = current->next;
		if (current == g_list.tail)
		{
			g_list.tail = current->prev;
		}
		else
		{
			current->next->prev = current->prev;
		}
	}
	free(current);
	g_list.num_nodes--;
	printf("Data %d found and removed.\n", data);
	return EXIT_SUCCESS;
}


int remove_elem(list_node * elem_to_remove)
{
	if (NULL == elem_to_remove)
	{
		printf("Error - Invalid element to remove.\n");
		return EXIT_FAILURE;
	}
	else if (0 == g_list.num_nodes)
	{
		printf("Error - Empty list.\n");
		return EXIT_FAILURE;
	}
	else
	{
		if (elem_to_remove == g_current_elem)
		{
			g_current_elem = elem_to_remove->next;
		}

		if (g_list.head == elem_to_remove)
		{
			g_list.head = elem_to_remove->next;
			if (NULL != elem_to_remove->next)
			{
				elem_to_remove->next->prev = NULL;
			}
			else
			{
				g_list.tail = NULL;
			}
		}
		else
		{
			elem_to_remove->prev->next = elem_to_remove->next;
			if (NULL == elem_to_remove->next)
			{
				g_list.tail = elem_to_remove->prev;
			}
			else
			{
				elem_to_remove->next->prev = elem_to_remove->prev;

			}
		}

		free(elem_to_remove);
		--g_list.num_nodes;
	}

	return EXIT_SUCCESS;
}


int list_sort(bool is_ascending)
{
	list_node * current = NULL;
	list_node * temp_head = NULL;
	list_node * temp_tail = NULL;
	list_node * node_to_move = NULL;
	bool inserted = false;
	bool sort_condition;

	if (0 == g_list.num_nodes)
	{
		printf("Empty list - no sorting will be done.\n");
	}
	else
	{
		do
		{
			node_to_move = g_list.head;
			g_list.head = node_to_move->next;
			if (NULL == temp_head)
			{
				temp_head = node_to_move;
				temp_tail = node_to_move;
				node_to_move->prev = NULL;
				node_to_move->next = NULL;
				continue;
			}
			current = temp_head;
			while (NULL != current)
			{
				sort_condition = is_ascending ? (current->data > node_to_move->data) : (current->data < node_to_move->data);
				if (sort_condition)
				{
					node_to_move->next = current;
					node_to_move->prev = current->prev;
					if (current == temp_head)
					{
						temp_head = node_to_move;
					}
					else
					{
						current->prev->next = node_to_move;
					}
					current->prev = node_to_move;
					inserted = true;
					break;
				}
				current = current->next;
			}

			if (true != inserted)
			{
				node_to_move->prev = temp_tail;
				node_to_move->next = NULL;
				temp_tail->next = node_to_move;
				temp_tail = node_to_move;
			}
			inserted = false;
		}while(g_list.head != NULL);

		g_list.head = temp_head;
		g_list.tail = temp_tail;
	}
	return EXIT_SUCCESS;
}


int list_display()
{
	list_node * current;

	if (0 == g_list.num_nodes)
	{
		printf("List empty - no elements will be printed.\n");
	}
	else
	{
		current = g_list.head;
		printf("List display >>>>>>>>>>>>>>>>>>>>>>>\n");
		do
		{
			printf("%d\n", current->data);
			current = current->next;
		}while(NULL != current);
		printf("List display <<<<<<<<<<<<<<<<<<<<<<<\n");
	}
	return EXIT_SUCCESS;
}


int list_destroy()
{
	list_node * current;

	if (0 != g_list.num_nodes)
	{
		do
		{
			current = g_list.head;
			g_list.head = current->next;
			free(current);
		}while(NULL != g_list.head);
	}
	g_list.num_nodes = 0;
	g_current_elem = NULL;
	printf("List destroyed.\n");
	return EXIT_SUCCESS;
}


int display_help(char option)
{
	switch (option)
	{
		case 'x':
		case 'a':
			printf("Linkit! Copyright: Murphy Meng 2011.\n");
			printf("NAME:\n");
			printf("	a - add data to given position.\n");
			printf("Description:\n");
			printf("	ah:\n");
			printf("	  Add data as the head of the list.\n");
			printf("	at:\n");
			printf("	  Add data as the tail of the list.\n");
			printf("	ap:\n");
			printf("	  Add data as the previous element.\n");
			printf("	an:\n");
			printf("	  Add data as the next element.\n");
			if ('x'!= option) break;

		case 'r':
			printf("NAME:\n");
			printf("	r - remove element from given position in the list.\n");
			printf("Description:\n");
			printf("	rc:\n");
			printf("	  Remove the current element.\n");
			printf("	rp:\n");
			printf("	  Remove the previous element.\n");
			printf("	rn:\n");
			printf("	  Remove the next element.\n");
			printf("	ra:\n");
			printf("	  Remove all elements, the list will be empty.\n");
			if ('x'!= option) break;

		case 'p':
			printf("NAME:\n");
			printf("	p - Print out the data in the given position of the list.\n");
			printf("Description:\n");
			printf("	pc:\n");
			printf("	  print out the current element.\n");
			printf("	pp:\n");
			printf("	  Print out the previous element.\n");
			printf("	pn:\n");
			printf("	  Print out the next element.\n");
			printf("	pa:\n");
			printf("	  Print out the list.\n");
			if ('x'!= option) break;

		case 'm':
			printf("NAME:\n");
			printf("	m - move to a given position in the list.\n");
			printf("Description:\n");
			printf("	mh:\n");
			printf("	  Move to the head of the list.\n");
			printf("	mt:\n");
			printf("	  Move to the tail of the list.\n");
			printf("	mp:\n");
			printf("	  Move to the previous element.\n");
			printf("	mn:\n");
			printf("	  Move to the next element.\n");
			if ('x'!= option) break;

		case 'i':
			printf("NAME:\n");
			printf("	i - check the current position.\n");
			printf("Description:\n");
			printf("	ih:\n");
			printf("	  Return yes if you are currently at the head of list, otherwise no.\n");
			printf("	mt:\n");
			printf("	  Return yes if you are currently at the tail of list, otherwise no.\n");
			if ('x'!= option) break;

		case 's':
			printf("NAME:\n");
			printf("	s - Rearrange of elements in list in the given order.\n");
			printf("Description:\n");
			printf("	sa:\n");
			printf("	  Sort the list in ascending order.\n");
			printf("	mt:\n");
			printf("	  Sort the list in descending order.\n");
			if ('x'!= option) break;

		case 'c':
			printf("NAME:\n");
			printf("	c - count the number of elements in the list.\n");
			printf("Description:\n");
			printf("	c:\n");
			printf("	  Print out the total number of elements of the list.\n");
			if ('x'!= option) break;

		default:
			break;
			/*Should never get here.*/
	}
	return EXIT_SUCCESS;
}


int test(void)
{
	int fresh_data[] = {2,5,7,2,1,6,3,4,8,3,9};
	int i;
	int result = EXIT_FAILURE;

	result = list_init();
	if (EXIT_SUCCESS != result)
	{
		printf("Error - list initialization.\n");
		return EXIT_FAILURE;
	}

	printf("List initialized.\n");
	for (i = 0; i < 11; ++i)
	{
		result = insert_tail(fresh_data[i]);
		if (EXIT_SUCCESS!= result)
		{
			printf("Error - list insertion.\n");
			return EXIT_FAILURE;
		}
		printf("Data %d inserted.\n", fresh_data[i]);
	}

	printf("There are %d elements totally in list.\n", g_list.num_nodes);

	list_display();
printf("To sort the list.\n");
	list_sort(true);
	list_display();
printf("Is the list sorted and displayed?\n");
	list_destroy();

	return EXIT_SUCCESS;
}

int main()
{
	int count = 0;
	int data = 0;
	int i;
	int result = EXIT_FAILURE;
	char line[100];
	char answer;
	char cmd_main, cmd_sec;

	result = list_init();
	if (EXIT_SUCCESS != result)
	{
		printf("Error - failed to initialize list. Aborted.\n");
		return EXIT_FAILURE;
	}

	printf(">>>>>Doubly-linked list demo<<<<<\n");
	printf("1. Input data to form a list.\n");
	printf("How many numbers are you going to input?\n");
	fgets(line, sizeof(line), stdin);
	sscanf(line, "%d", &count);
	if (count < 1)
	{
		printf("You got be kidding me...Game over!\n");
		return EXIT_FAILURE;
	}
	printf("and they are...\n");
	for (i = 0; i < count; ++i)
	{
		fgets(line, sizeof(line), stdin);
		sscanf(line, "%d", &data);
		result = insert_tail(data);
		if (EXIT_SUCCESS != result)
		{
			printf("Error - failed to insert data into list. Aborted.\n");
			return EXIT_FAILURE;
		}
	}
	printf("List is ready.\n");

	printf("2. Print the whole list?\n");
	fgets(line, sizeof(line), stdin);
	list_display();

	printf("3. Remove data from list.\n");
	data = 0xFFFFFFFF;
	do
	{
		printf("Which data would you like to remove?\n");
		fgets(line, sizeof(line), stdin);
		sscanf(line, "%d", &data);
		result = data_remove(data);
		if (EXIT_SUCCESS != result)
		{
			printf("Error - failed to remove data from list. Aborted.\n");
			return EXIT_FAILURE;
		}
		printf("Print out the list to check?\n");
		fgets(line, sizeof(line), stdin);
		list_display();
		printf("Remove another data? (y/n)\n");
		fgets(line, sizeof(line), stdin);
		sscanf(line, "%c", &answer);
		if ('n' == answer)
		{
			break;
		}
		else if ('y' != answer)
		{
			printf("Input unrecognized. Remove another data? (y/n)\n");
			fgets(line, sizeof(line), stdin);
			sscanf(line, "%c", &answer);
			if ('n' == answer)
			{
				break;
			}
		}
		else {;}
	}while (1);

	printf("4. Add more data into the list.\n");
	data = 0xFFFFFFFF;
	do
	{
		printf("Give a data you would like to add into the list.\n");
		fgets(line, sizeof(line), stdin);
		sscanf(line, "%d", &data);
		if (0xFFFFFFFF == data)
		{
			break;
		}

		result = insert_tail(data);
		if (EXIT_SUCCESS != result)
		{
			printf("Error - failed to insert data into list. Aborted.\n");
			return EXIT_FAILURE;
		}
		printf("Print out the list to check?\n");
		fgets(line, sizeof(line), stdin);
		list_display();
		printf("Add another data? (y/n)\n");
		fgets(line, sizeof(line), stdin);
		sscanf(line, "%c", &answer);
		if ('n' == answer)
		{
			break;
		}
		else if ('y' != answer)
		{
			printf("Input unrecognized. Remove another data? (y/n)\n");
			fgets(line, sizeof(line), stdin);
			sscanf(line, "%c", &answer);
			if ('n' == answer)
			{
				break;
			}
		}
		else {;}
	}while(1);

	printf("5. Sort the list?\n");
	fgets(line, sizeof(line), stdin);
	list_sort(false);
	printf("Print out the list to check?\n");
	fgets(line, sizeof(line), stdin);
	list_display();

	printf("6. Play time.\n");
	printf("Enter any commands; \"h\" for help; \"q\" to terminate.\n");
	do
	{
		printf("Linkit!>");
		fgets(line, sizeof(line), stdin);
		sscanf(line, "%c%c", &cmd_main, &cmd_sec);
		printf("\n");
		switch (cmd_main)
		{
			case 'm':
				switch (cmd_sec)
				{
					case 'h':
						if (g_list.head != g_current_elem)
						{
							g_current_elem = g_list.head;
						}
						break;
					case 't':
						if (g_list.tail != g_current_elem)
						{
							g_current_elem = g_list.tail;
						}
						break;
					case 'n':
						if (NULL == g_current_elem)
						{
							printf("Warn - Out of bond. Move to head/tail firstly. \n");
						}
						else if (NULL != g_current_elem->next)
						{
							g_current_elem = g_current_elem->next;
						}
						else
						{
							printf("Warn - You are at the tail of list!\n");
						}
						break;
					case 'p':
						if (NULL == g_current_elem)
						{
							printf("Warn - Out of bond. Move to head/tail firstly. \n");
						}
						else if(NULL != g_current_elem->prev)
						{
							g_current_elem = g_current_elem->prev;
						}
						else
						{
							printf("Warn - You are at the head of list!\n");
						}
						break;
					default:
						printf("Wrong command.\n");
						display_help('m');
						break;
				}
				break;
			case 'p':
				switch (cmd_sec)
				{
					case 'c':
						if (NULL != g_current_elem)
						{
							printf("Current data = %d.\n", g_current_elem->data);
						}
						else
						{
							printf("Warn - Invalid current element.\n");
						}
						break;
					case 'p':
						if (NULL == g_current_elem)
						{
							printf("Warn - Out of bond. Move to head/tail firstly. \n");
						}
						else if (NULL != g_current_elem->prev)
						{
							printf("Previous data = %d.\n", g_current_elem->prev->data);
						}
						else
						{
							printf("Warn - You are at the head of list!\n");
						}
						break;
					case 'n':
						if (NULL == g_current_elem)
						{
							printf("Warn - Out of bond. Move to head/tail firstly. \n");
						}
						else if (NULL != g_current_elem->next)
						{
							printf("Next data = %d.\n", g_current_elem->next->data);
						}
						else
						{
							printf("Warn - You are at the tail of list!\n");
						}
						break;
					case 'a':
						list_display();
						break;
					default:
						printf("Wrong command.\n");
						display_help('p');
						break;
				}
				break;
			case 'r':
				switch (cmd_sec)
				{
					case 'c':
						result = remove_elem(g_current_elem);
						if (EXIT_SUCCESS != result)
						{
							printf("Error - Out of bond.\n");
						}
						break;
					case 'p':
						if (NULL == g_current_elem)
						{
							printf("Warn - Out of bond. Move to head/tail firstly. \n");
						}
						else if (NULL != g_current_elem->prev)
						{
							result = remove_elem(g_current_elem->prev);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - Out of bond.\n");
							}
						}
						else
						{
							printf("Warn - You are at the head of list!\n");
						}
						break;
					case 'n':
						if (NULL == g_current_elem)
						{
							printf("Warn - Out of bond. Move to head/tail firstly. \n");
						}
						else if (NULL != g_current_elem->next)
						{
							result = remove_elem(g_current_elem->next);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - Out of bond.\n");
							}
						}
						else
						{
							printf("Warn - You are at the tail of list!\n");
						}
						break;
					case 'a':
						result = list_destroy();
						if (EXIT_SUCCESS != result)
						{
							printf("Error - failed to destory the whole list. Aborted.\n");
							return EXIT_FAILURE;
						}
						break;
					default:
						printf("Wrong command.\n");
						display_help('r');
						break;
				}
				break;
			case 'a':
				printf("What data to add?");
				fgets(line, sizeof(line), stdin);
				sscanf(line, "%d", &data);
				printf("\n");
				switch (cmd_sec)
				{
					case 'h':
						result = insert_head(data);
						if (EXIT_SUCCESS != result)
						{
							printf("Error - failed to insert data into list. Aborted.\n");
							return EXIT_FAILURE;
						}
						break;
					case 't':
						result = insert_tail(data);
						if (EXIT_SUCCESS != result)
						{
							printf("Error - failed to insert data into list. Aborted.\n");
							return EXIT_FAILURE;
						}
						break;
					case 'p':
						result = insert_prev(data);
						if (EXIT_SUCCESS != result)
						{
							printf("Error - failed to insert data into list. Aborted.\n");
							return EXIT_FAILURE;
						}
						break;
					case 'n':
						result = insert_next(data);
						if (EXIT_SUCCESS != result)
						{
							printf("Error - failed to insert data into list. Aborted.\n");
							return EXIT_FAILURE;
						}
						break;
					default:
						printf("Wrong command.\n");
						display_help('a');
						break;
				}
				break;
			case 's':
				switch (cmd_sec)
				{
					case 'a':
						list_sort(true);
						break;
					case 'd':
						list_sort(false);
						break;
					default:
						printf("Wrong command.\n");
						display_help('s');
						break;
				}
				break;
			case 'i':
				if ((NULL == g_list.head) || (NULL == g_list.tail))
				{
					printf("Empty list!\n");
				}
				else
				{
					switch (cmd_sec)
					{
						case 'h':
							printf("Currently at the head?");
							if (g_current_elem == g_list.head)
							{
								printf("yes.\n");
							}
							else
							{
								printf("no.\n");
							}
							break;
						case 't':
							printf("Currently at the tail?");
							if (g_current_elem == g_list.tail)
							{
								printf("yes.\n");
							}
							else
							{
								printf("no.\n");
							}
							break;
						default:
							printf("Wrong command.\n");
							display_help('i');
							break;
					}
				}
				break;
			case 'c':
				printf("There are %d elements in the list.\n", g_list.num_nodes);
				break;
			case 'q':
				break;
			case 'h':
				display_help('x');
				break;
			default:
				printf("Wrong command.\n");
				display_help('x');
				break;
		}
	}while('q' != cmd_main);

	printf("7. Destroy the list.\n");
	fgets(line, sizeof(line), stdin);
	list_destroy();
	printf("Print out the list to check?\n");
	fgets(line, sizeof(line), stdin);
	list_display();

	printf("Test finished.\n");
	return EXIT_SUCCESS;
}
