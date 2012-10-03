/*
 ============================================================================
 Name        : hashit.c
 Author      : Murphy Meng
 Version     :
 Copyright   : Murphy Meng 2012
 Description : Hash table test suite
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "inc/hashtable.h"
#include "inc/common.h"

#define NUM_CW 7U
#define	CREATE  0U
#define ADD		1U
#define	DEL		2U
#define	QUERY	3U
#define	INFO	4U
#define	DESTROY 5U
#define HELP	6U

char control_world[NUM_CW][8U] =
{
	"create",
	"add",
	"del",
	"query",
	"info",
	"destroy",
	"help"
};

#define NUM_ELEM 21U

char *f_data[NUM_ELEM] =
	{	"qhqw",
		"ashg",
		"9807yy",
		"kjchqi2983 ",
		"\n&",
		"~ ¬h",
		"0836873231298349012342038",
		"	1",
		"	",
		"234",
		"g3442",
		"38uht",
		" eth3wt2",
		"2 321",
		" g2",
		"26u q",
		"wrvdq",
		"b47i",
		"n 3563",
		"fghe",
		"5"};

void removal(void *p_data);

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

void display(void *p_key, void *p_data)
{
	printf("\t%s\t%s", (char *)p_key, (char *)p_data);
}

void removal(void *p_data)
{

}

uint32_t time33(void *p)
{
	uint32_t i;
	uint32_t hash = 0;
	char *str = (char *)p;
	uint32_t len = strlen(str);

	for (i = 0; i < len; i++)
	{
		hash = hash *33 + (uint32_t)str[i];
	}

	return hash%NUM_BUCKETS;
}

void display_help();

SYSTEM_STATUS test_hashtable()
{
	uint32_t i, probe_count = 0U;
	int32_t location;
	uint8_t handle = 0U;
	SYSTEM_STATUS status = STATUS_FAIL;
	char *key;
	char *str;

	status = HT_create(time33,
					   compare,
					   display,
					   removal,
					   &handle);
	assert(status == STATUS_OK);

	for (i = 0U; i < NUM_ELEM-1; i++)
	{
		DIAG_INFO("[%d]Inserting %s.", i, f_data[i]);
		status = HT_insert(handle,
							f_data[i],
					  	    f_data[i],
					  	    &probe_count,
					  	    &location);
		printf("probe_count = %d; location = %d；\n", probe_count, location);
		assert(status == STATUS_OK);
	}

	status = HT_insert(handle,
						f_data[NUM_ELEM-1],
				  	    f_data[NUM_ELEM-1],
				  	    &probe_count,
				  	    &location);
		printf("probe_count = %d; location = %d\n", probe_count, location);
	assert(status == STATUS_DATABASE_FULL);

	status = HT_traverse(handle);
	assert(status == STATUS_OK);

	key = "ashg";
	DIAG_INFO("Removing entry with key \"%s\"", key);
	status = HT_remove(handle, key, &probe_count, &location);
		printf("probe_count = %d; location = %d\n", probe_count, location);
	assert(status == STATUS_OK);

	key = "\n&";
	status = HT_lookup(handle, key, &str, &probe_count, &location);
		printf("probe_count = %d; location = %d\n", probe_count, location);
	assert(status == STATUS_OK);
	DIAG_INFO("What I got is %s.", str);

	key = "~ ¬h";
	DIAG_INFO("Removing entry with key \"%s\"", key);
	status = HT_remove(handle, key, &probe_count, &location);
		printf("probe_count = %d; location = %d\n", probe_count, location);
	assert(status == STATUS_OK);

	key = "38uht";
	status = HT_lookup(handle, key, &str, &probe_count, &location);
		printf("probe_count = %d; location = %d\n", probe_count, location);
	assert(status == STATUS_OK);
	DIAG_INFO("What I got is %s.", str);

	status = HT_traverse(handle);
	assert(status == STATUS_OK);

	status = HT_destroy(handle);
	assert(status == STATUS_OK);

	DIAG_INFO("Test finished.");

	return status;
}

int main(void)
{
	SYSTEM_STATUS status = STATUS_FAIL;
	cmd_line_tag cmd_line;
	char line[100];
	char *p_data = NULL;
	uint8_t handle = 0U;
	uint32_t probe_count = 0U;
	int32_t location = -1;
	uint32_t cw, i;

	printf("--BelikeTree-- @Murphy Meng 2012\n");
	printf("Type help for more info.\n");

#if 0
	test_hashtable();

	printf("cmd = %s\n", cmd_line.cmd);
	printf("num option = %d\n", cmd_line.num_options);
	printf("options = %s\n", cmd_line.option);
	printf("num params = %d\n", cmd_line.num_params);
	for (i = 0U; i < cmd_line.num_params; i++)
	{
	printf("params = %s\n", cmd_line.params[i]);
	}
#endif

	do
	{
		printf("Hashit! 8->");
		fgets(line, sizeof(line), stdin);
		fgets_convert(line);
		printf("\n");

		status = parse_cmd(line, &cmd_line);

		for (cw = 0U; cw < NUM_CW; cw++)
		{
			if (!strcmp(cmd_line.cmd, control_world[cw]))
			{
				break;
			}
		}

		if (NUM_CW == cw)
		{
			if (strcmp(cmd_line.cmd, "quit"))
			{
				printf("Error - invalid command.\n");
			}
			continue;
		}
		switch (cw)
		{
			case CREATE:
				if ((0 != cmd_line.num_options) || (0 != cmd_line.num_params))
				{
					printf("Invalid format for create.");
				}
				else
				{
					status = HT_create(time33,
									   compare,
									   display,
									   removal,
									   &handle);
					if (STATUS_OK != status)
					{
						DIAG_ERROR("HT_create() failed (%#x).", status);
					}
				}
				break;

			case ADD:
				if (0 != cmd_line.num_params % 2)
				{
					printf("Invalid format for create\n.");
					continue;
				}
				else
				{
					for (i = 0U; i < cmd_line.num_params; i = i + 2U)
					{
						status = HT_insert(handle,
										   cmd_line.params[i],
										   cmd_line.params[i+1U],
										   &probe_count,
										   &location);
						if ((STATUS_OK == status) || (STATUS_DATABASE_OVERWRITEN == status))
						{
							printf("Insertion succeeds.\n");
						}
						else if (STATUS_FAIL == status)
						{
							printf("Fail to insert key %s.\n", cmd_line.params[i]);
							continue;
						}
						else
						{}

						if ((0 != cmd_line.num_options) && ('i' == *cmd_line.option))
						{

							if (STATUS_DATABASE_DUPLICATED == status)
							{
								printf("Data with key %s exists.", cmd_line.params[i]);
							}
							else if (STATUS_DATABASE_OVERWRITEN == status)
							{
								printf("Data with key %s updated", cmd_line.params[i]);
							}
							else
							{}

							printf("The hash is %d.\n", time33(cmd_line.params[i]));
							if (0 != probe_count)
							{
								printf("Probing for %d times, \n", probe_count);
							}
							printf("Data is stored at bucket[%d].\n", location);
						}
					}
				}
				break;

			case DEL:
				for (i = 0U; i < cmd_line.num_params; i++)
				{
					status = HT_remove(handle, cmd_line.params[i], &probe_count, &location);
					if (STATUS_FAIL == status)
					{
						printf("Fail to remove key %s.", cmd_line.params[i]);
					}
					else if (STATUS_OK == status)
					{
						printf("Removal succeeds.\n");
					}

					if ((0 != cmd_line.num_options) && ('i' == *cmd_line.option))
					{

						if (STATUS_DATABASE_NOT_FOUND == status)
						{
							printf("Data with key %s not found.", cmd_line.params[i]);
						}
						else
						{
							printf("Data is stored at bucket[%d].", location);
						}

						printf("The hash is %d.\n", time33(cmd_line.params[i]));
						if (0 != probe_count)
						{
							printf("Probing for %d times, \n", probe_count);
						}
					}
				}
				break;

			case QUERY:

				for (i = 0U; i < cmd_line.num_params; i++)
				{
					status = HT_lookup(handle, cmd_line.params[i], &p_data, &probe_count, &location);
					if (STATUS_FAIL == status)
					{
						printf("Fail to query key %s.", cmd_line.params[i]);
					}
					else if (STATUS_OK == status)
					{
						printf("Query succeeds.\n");
					}

					if ((0 != cmd_line.num_options) && ('i' == *cmd_line.option))
					{

						if (STATUS_DATABASE_NOT_FOUND == status)
						{
							printf("Data with key %s not found.\n", cmd_line.params[i]);
						}
						else
						{
							printf("Data %s is stored at bucket[%d].\n", p_data, location);
						}

						printf("The hash is %d.\n", time33(cmd_line.params[i]));
						if (0 != probe_count)
						{
							printf("Probing for %d times, \n", probe_count);
						}
					}
				}
				break;

			case INFO:
				if ((0 != cmd_line.num_options) || (0 != cmd_line.num_params))
				{
					printf("Invalid format for info.");
				}
				else
				{
					status = HT_traverse(handle);
					if (STATUS_OK != status)
					{
						DIAG_ERROR("HT_destroy() failed (%#x).", status);
					}
				}
				break;

			case DESTROY:
				if ((0 != cmd_line.num_options) || (0 != cmd_line.num_params))
				{
					printf("Invalid format for destroy.");
					continue;
				}
				else
				{
					status = HT_destroy(handle);
					if (STATUS_OK != status)
					{
						DIAG_ERROR("HT_destroy() failed (%#x).", status);
					}
				}
				break;

			case HELP:
				display_help();
				break;

			default:
				printf("Wrong command.\n");
				break;
		}
	}while(strcmp(cmd_line.cmd, "quit") && (STATUS_FAIL != status));

	return 0;
}

void display_help()
{
	printf("--Hashit!-- @Murphy Meng 2012\n");
	printf("Command format： command -[option] parameter_list\n");
	printf("	create  - create a hash table. No option or other parameters available.\n");
	printf("	add     - add new entry in hash table. Option -i (optional) is supported for more info. Parameters must be in the form of key-value pairs separated by space. No space within a string. \n");
	printf("				i.e. add -i key1 value1 key2 value2\n");
	printf("	del     - remove an entry from hash table. Option -i (optional) is also supported. Parameters must be keys seperated by space. \n");
	printf("				i.e. del -i key1 key2\n");
	printf("	query   - look up an entry in hash table. Option -i (optional) is also supported. Parameters must be keys seperated by space.\n");
	printf("				i.e. query -i key1 key2\n");
	printf("	info	- Display all entries in hash table and other useful info.\n");
	printf("	destroy - destroy the hash table.\n");
}
