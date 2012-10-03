/*
 * common.c
 *
 *  Created on: 2012-3-11
 *      Author: singlaive
 */

#include <stdlib.h>
#include <string.h>
#include "common.h"

void fgets_convert(char * str, char * str1)
{
	char * ptr = str;

	while (('\n' != *ptr) && ('\n' != *ptr))
	{
		ptr++;
	}

	if ('\n' == *ptr)
	{
		*ptr = 0;
	}

	strcpy(str1, str);
}

int COMMON_handle_create(void *ptr, uint8_t *p_handle)
{
	static uint8_t counter = 0U;

	p_trees[counter] = ptr;
	*p_handle = HANDLE_BASE_TREE | counter;
	counter++;

	return EXIT_SUCCESS;
}

int COMMON_object_retrieve(uint8_t handle, void **ptr)
{
	uint32_t index = handle & HANDLE_COUNTER_MASK;
	*ptr = p_trees[index];

	return EXIT_SUCCESS;
}
