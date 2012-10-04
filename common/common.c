/*
 * common.c
 *
 *  Created on: 2012-3-11
 *      Author: singlaive
 */

/* Modification history:
*     11th Apr 2012 Tree implementation
 *     6th May 2012 Hash table implementation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

/* Change to strtok_r since we shall use it to more than one string at the same time. */
SYSTEM_STATUS parse_cmd(char *str, cmd_line_tag *p_commend_line)
{
	SYSTEM_STATUS status = STATUS_OK;
	char *ptr[100U];
	uint32_t i = 0U;
	uint32_t param_counter = 0U;

	memset(ptr, '\0', 100U);
	memset(p_commend_line, '\0', sizeof(cmd_line_tag));
	while(NULL != (ptr[i++] = strtok(str, " ")))
	{
		str = NULL;
		param_counter++;
	}

	param_counter--;

	if (MAX_CMD_LENGTH < strlen(ptr[0U]))
	{
		status = STATUS_FAIL;
		DIAG_ERROR("Commend too long (larger then %d characters).", MAX_CMD_LENGTH);
	}
	else
	{
		strcpy(p_commend_line->cmd, ptr[0U]);
	}

	if ((STATUS_OK == status) && (param_counter > 0) && ('-' == *(ptr[1U])))
	{
		/* There shall be no '\0' to terminate the option array. */
		p_commend_line->num_options = strlen(ptr[1U]) - 1U;
		p_commend_line->option = malloc(p_commend_line->num_options*sizeof(char));
		if (NULL == p_commend_line->option)
		{
			status = STATUS_FAIL;
			DIAG_ERROR("Malloc failed.");
		}
		else
		{
			strcpy(p_commend_line->option, (ptr[1U]+1U));
			param_counter--;
		}
	}

	if ((STATUS_OK == status) && (param_counter > 0))
	{
		i = (NULL == p_commend_line->option) ? 1U : 2U;
		p_commend_line->num_params = param_counter;

		p_commend_line->params = malloc(param_counter*sizeof(char*));
		param_counter = 0U;

		for (; (NULL != ptr[i]); i++)
		{
			uint32_t param_length = 0U;

			if (('\"' == *(ptr[i])) && ('\"' == ptr[i][strlen(ptr[i])-1U]))
			{
				param_length = strlen(ptr[i]) - 2U;
				ptr[i]++;
			}
			else if (('\"' != *(ptr[i])) && ('\"' != ptr[i][strlen(ptr[i])-1U]))
			{
				param_length = strlen(ptr[i]);
			}
			else
			{
				status = STATUS_FAIL;
			}

			if (STATUS_OK == status)
			{
				/* Allocate memory including the string terminator. */
				p_commend_line->params[param_counter] = malloc((param_length+1U)*sizeof(char));
				if (NULL == p_commend_line->params[param_counter])
				{
					DIAG_ERROR("Malloc failed.");
					status = STATUS_FAIL;
				}
				else
				{
					strncpy(p_commend_line->params[param_counter], ptr[i], param_length);
					p_commend_line->params[param_counter][param_length+1U] = '\0';
				}

				param_counter++;
			}
		}
	}

	return status;
}

void fgets_convert(char * str)
{
	char * ptr = str;

	while (('\n' != *ptr) && ('\0' != *ptr))
	{
		ptr++;
	}

	if ('\n' == *ptr)
	{
		*ptr = '\0';
	}
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
