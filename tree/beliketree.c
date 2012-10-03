/*
 * beliketree.c
 *
 *  Created on: 2012-3-27
 *      Author: singlaive
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "tree.h"

#define NUM_CW 7U
#define	CREATE  0U
#define ADD		1U
#define	DEL		2U
#define	FIND	3U
#define	DISPLAY 4U
#define	DESTROY 5U
#define HELP	6U

int data_bst[] = {4, 8, 3, 2, 10, 55, 21, 0, 7, 35, 22, 1, 9};
int data_avl[] = {100, 50, 30, 16, 40, 8, 12, 14, 24, 4, 13, 200, 20, 18};

char control_world[NUM_CW][8U] =
{
	"create",
	"add",
	"del",
	"find",
	"display",
	"destroy",
	"help"
};

void display_help(void);

int32_t compare(void *key1, void *key2)
{
	int32_t result;
	int32_t *int_value1 = (int *)key1;
	int32_t *int_value2 = (int *)key2;

	if (*int_value1 == *int_value2)
	{
		result = 0;
	}
	else if (*int_value1 < *int_value2)
	{
		result = -1;
	}
	else
	{
		result = 1;
	}

	return result;
}

void display(void *data)
{
	printf("%d\n", *(int *)data);
}

void Bstree_test(void)
{
	int32_t status;
	uint32_t i, int_value;
	uint32_t data_size;
	uint8_t tree_handle;
	bool is_found = false;

	status = BStree_create(&tree_handle, compare, display, NULL);
	assert(status == EXIT_SUCCESS);

	data_size = sizeof(data_bst)/sizeof(int);
	for (i = 0; i < data_size; i++)
	{
		status = BStree_attach(tree_handle, data_bst+i);
		assert(status == EXIT_SUCCESS);
	}

	status = BStree_traverse(tree_handle, TRAVERSE_METHOD_INORDER);
	assert(status == EXIT_SUCCESS);

	printf("===\n");

	status = BStree_traverse(tree_handle, TRAVERSE_METHOD_PREORDER);
	assert(status == EXIT_SUCCESS);
	printf("===\n");

	status = BStree_traverse(tree_handle, TRAVERSE_METHOD_POSTORDER);
	assert(status == EXIT_SUCCESS);
	printf("===\n");

	int_value = 21;
	status = BStree_lookup(tree_handle, (void *)&int_value, &is_found);
	printf("Data %d is found: %d\n", int_value, is_found);

	status = BStree_remove(tree_handle, (void *)&int_value);
	assert(status == EXIT_SUCCESS);
	printf("Remove OK %d\n", status);
	status = BStree_traverse(tree_handle, TRAVERSE_METHOD_PREORDER);
	assert(status == EXIT_SUCCESS);

	status = BStree_destroy(tree_handle);
	assert(status == EXIT_SUCCESS);
	status = BStree_traverse(tree_handle, TRAVERSE_METHOD_PREORDER);
	assert(status == EXIT_SUCCESS);
}

void AVLtree_test()
{
	int32_t status;
	uint32_t i, int_value;
	uint32_t data_size;
	uint8_t tree_handle;

	status = AVLtree_create(&tree_handle, compare, display, NULL);
	assert(status == EXIT_SUCCESS);

	data_size = sizeof(data_avl)/sizeof(int);
	for (i = 0; i < data_size; i++)
	{
		printf("Info - Inserting %d\n", data_avl[i]);
		status = AVLtree_attach(tree_handle, data_avl+i);
		assert(status == EXIT_SUCCESS);

		printf("After asserting: >>\n");
		status = BStree_traverse(tree_handle, TRAVERSE_METHOD_PREORDER);
		assert(status == EXIT_SUCCESS);
		printf("<<<<<<<<<<<<<<<<<<<\n");
	}

	printf("Infor - removing 13 from the tree.\n");
	int_value = 13;
	status = AVLtree_remove(tree_handle, &int_value);
	assert(status == EXIT_SUCCESS);

	printf("Info - The tree becomes:\n");
	status = BStree_traverse(tree_handle, TRAVERSE_METHOD_PREORDER);
	assert(status == EXIT_SUCCESS);

	printf("Infor - removing 14 from the tree.\n");
	int_value = 14;
	status = AVLtree_remove(tree_handle, &int_value);
	assert(status == EXIT_SUCCESS);

	printf("Info - The tree becomes:\n");
	status = BStree_traverse(tree_handle, TRAVERSE_METHOD_PREORDER);
	assert(status == EXIT_SUCCESS);

}

void RBtree_test()
{
	int32_t status;
	uint32_t i, int_value;
	uint32_t data_size;
	uint8_t tree_handle;

	status = RBtree_create(&tree_handle, compare, display, NULL);
	assert(status == EXIT_SUCCESS);

	data_size = sizeof(data_avl)/sizeof(int);
	for (i = 0; i < data_size; i++)
	{
		printf("Info - Inserting %d\n", data_avl[i]);
		status = RBtree_attach(tree_handle, data_avl+i);
		assert(status == EXIT_SUCCESS);
#if 0
		printf("After asserting: >>\n");
		status = RBtree_traverse(tree_handle, TRAVERSE_METHOD_PREORDER);
		assert(status == EXIT_SUCCESS);
		printf("<<<<<<<<<<<<<<<<<<<\n");
#endif
	}

	int_value = 100;
	printf("Remove %d...\n", int_value);
	status = RBtree_remove(tree_handle, &int_value);
	assert(status == EXIT_SUCCESS);

	status = RBtree_traverse(tree_handle, TRAVERSE_METHOD_PREORDER);
	assert(status == EXIT_SUCCESS);
	printf("Removed.\n");
	(void)RBtree_sanity_check(tree_handle);
}
int main(void)
{
	char cmd[10U];
	char line[100];
	int rt;
	int result = EXIT_FAILURE;
	int *p_int_value[100U];
	uint32_t node_count = 0U;
	uint32_t cw, i;
	char tree_type = 0U;
	uint8_t tree_handle;
	int int_value;
	char temp[20];

	printf("--BelikeTree-- @Murphy Meng 2012\n");
	printf("Type help for more info.\n");

	do
	{
		printf("BelikeTree 8->");
		memset(cmd, 0x00, 10U*sizeof(char));
		fgets(line, sizeof(line), stdin);
		sscanf(line, "%s %s", cmd, temp);
		printf("\n");

		for (cw = 0U; cw < NUM_CW; cw++)
		{
			if (!strcmp(cmd, control_world[cw]))
			{
				break;
			}
		}

		if (NUM_CW == cw)
		{
			if (strcmp(cmd, "q"))
			{
				printf("Error - invalid command.\n");
			}
			continue;
		}
		switch (cw)
		{
			case CREATE:
				rt = sscanf(temp, "-%c", &tree_type);
				if (rt != 1)
				{
					printf("Command invalid.\n");
					result = EXIT_SUCCESS;
				}
				else
				{
					switch (tree_type)
					{
						case 'b':
							printf("Create BST tree.\n");
							result = BStree_create(&tree_handle, compare, display, NULL);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - BStree_create failed.\n");
							}
							printf("finished.\n");
							break;
						case 'a':
							result = AVLtree_create(&tree_handle, compare, display, NULL);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - AVLtree_create failed.\n");
							}
							break;
						case 'r':
							result = RBtree_create(&tree_handle, compare, display, NULL);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - RBtree_create failed.\n");
							}
							break;
						default:
							printf("Error - unsupported option for create.\n");
							break;
					}
				}
				break;
			case ADD:
				p_int_value[node_count] = malloc(sizeof(int));
				rt = sscanf(temp, "%d", p_int_value[node_count]);
				if (rt != 1)
				{
					printf("Command invalid.\n");
					free(p_int_value[node_count]);
					result = EXIT_SUCCESS;
				}
				else
				{
					switch (tree_type)
					{
						case 'b':
							result = BStree_attach(tree_handle, p_int_value[node_count]);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - BStree_attach failed.\n");
							}
							break;
						case 'a':
							result = AVLtree_attach(tree_handle, p_int_value[node_count]);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - BStree_attach failed.\n");
							}
							break;
						case 'r':
							result = RBtree_attach(tree_handle, p_int_value[node_count]);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - BStree_attach failed.\n");
							}
							break;
						default:
							break;
					}

					node_count++;
				}
				break;
			case DEL:
				rt = sscanf(temp, "%d", &int_value);
				if (rt != 1)
				{
					printf("Command invalid.\n");
					free(p_int_value[node_count]);
					result = EXIT_SUCCESS;
				}
				else
				{
					switch (tree_type)
					{
						case 'b':
							result = BStree_remove(tree_handle, (void *)&int_value);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - BStree_remove failed.\n");
							}
							break;
						case 'a':
							result = AVLtree_remove(tree_handle, (void *)&int_value);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - BStree_attach failed.\n");
							}
							break;
						case 'r':
							result = RBtree_remove(tree_handle, (void *)&int_value);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - BStree_attach failed.\n");
							}
							break;
						default:
							break;
					}
				}
				break;
			case FIND:
			{
				bool is_found = false;

				rt = sscanf(temp, "%d", &int_value);
				if (rt != 1)
				{
					printf("Command invalid.\n");
					free(p_int_value[node_count]);
					result = EXIT_SUCCESS;
				}
				else
				{
					result = BStree_lookup(tree_handle, (void *)&int_value, &is_found);
					if (EXIT_SUCCESS != result)
					{
						printf("Error - BStree_attach failed.\n");
					}
					else
					{
						if (true == is_found)
						{
							printf("Got it!\n");
						}
						else
						{
							printf("Data not found.\n");
						}
					}
				}
				break;
			}
			case DISPLAY:
				rt = sscanf(temp, "-%d", &int_value);
				if (rt != 1)
				{
					printf("Command invalid.\n");
					result = EXIT_SUCCESS;
				}
				else
				{
					switch (tree_type)
					{
						case 'b':
						case 'a':
							result = BStree_traverse(tree_handle, int_value);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - Traversal failed.\n");
							}
							break;
						case 'r':
							result = RBtree_traverse(tree_handle, int_value);
							if (EXIT_SUCCESS != result)
							{
								printf("Error - Traversal failed.\n");
							}
							break;
						default:
							break;
					}
				}
				break;
			case DESTROY:
				result = BStree_destroy(tree_handle);
				if (EXIT_SUCCESS != result)
				{
					printf("Error - BStree_destroy failed.\n");
				}

				for (i = 0U; i < node_count; i++)
				{
					free(p_int_value[i]);
				}
				node_count = 0U;
				break;
			case HELP:
				display_help();
				result = EXIT_SUCCESS;
				break;
			default:
				printf("Wrong command.\n");
				break;
		}
	}while(strcmp(cmd, "q") && (EXIT_SUCCESS == result));

	return 0;
}


void display_help(void)
{
	printf("--BelikeTree-- @Murphy Meng 2012\n");
	printf("Command format： command -[option] data\n");
	printf("	create  - create a binary search tree. No data needs to be indicated.\n");
	printf("	  -b  a general binary search tree shall be created.\n");
	printf("	  -a  a height balanced tree (AVL tree) shall be created.\n");
	printf("	  -r  a red black tree shall be created.\n");
	printf("	add     - add new data in the tree. Only one data inserted at one time. Integer only. \n");
	printf("	del     - remove a data from the tree. Only one data deleted at one time. \n");
	printf("	find    - look up a data in the tree. One and only one data required.\n");
	printf("	display - traverse and print out all data in the tree. No data needs to be indicated.\n");
	printf("	  -1  in order traverse method shall be used.\n");
	printf("	  -2  pre order traverse method shall be used.\n");
	printf("	  -3  post order traverse method shall be used.\n");
	printf("	destroy - destroy the tree. No data needs to be indicated. \n");
}

