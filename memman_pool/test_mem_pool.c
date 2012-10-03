/*
 * test_mem_pool.c
 *
 *  Created on: 2011-11-17
 *      Author: singlaive
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "memman_pool.h"
#include "mem_common.h"

#define MAX_MALLOC_TIMES 10U
#define MEMORY_ALLOCATION_SIZE 100U

typedef struct _test_stuct
{
	uint32_t value_ID;
	uint32_t num_value;
	int *    values;
}test_struct;

typedef union _test_union
{
	uint32_t value_ID;
	test_struct value;
}test_union;

void main(void)
{
	char * ptr[MAX_MALLOC_TIMES];
	uint32_t i, index;
	test_union * p_union = NULL;
	test_struct * p_struct = NULL;
	SYSTEM_STATUS result = SYSTEM_STATUS_FAIL;
	struct timeval current_time, time_start, time_end;
	uint32_t time_interval_malloc = 0U;
	uint32_t time_interval_pool = 0U;

	do
	{
#if 0
		if(SYSTEM_STATUS_OK != mem_pool_init())
		{
			log_error("Failed to mem_pool_init.");
			break;
		}

		log_info("Memory pool initialized.");
		printf("sizeof(test_struct) = %d.\n", sizeof(test_struct));
#endif
		for (index = 0u; index < 1000; ++index)
		{
			gettimeofday(&current_time, NULL);
			time_start = current_time;

			for (i = 0U; i < MAX_MALLOC_TIMES; ++i)
			{
				ptr[i] = malloc(MEMORY_ALLOCATION_SIZE);
			}
			for (i = 0U; i < MAX_MALLOC_TIMES; ++i)
			{
				free(ptr[i]);
			}
			gettimeofday(&current_time, NULL);
			time_end = current_time;

			time_interval_malloc += time_diff(time_start, time_end);

			gettimeofday(&current_time, NULL);
			time_start = current_time;

			(void)mem_pool_init();
			for (i = 0U; i < MAX_MALLOC_TIMES; ++i)
			{

				ptr[i] = mem_alloc(MEMORY_ALLOCATION_SIZE);
			}

			for (i = 0U; i < MAX_MALLOC_TIMES; ++i)
			{
				mem_free(ptr[i]);
			}
			(void)mem_pool_destroy();
			gettimeofday(&current_time, NULL);
			time_end = current_time;

			time_interval_pool += time_diff(time_start, time_end);
		}

		printf("Malloc costs %d usec.\n", time_interval_malloc/1000);
		printf("Memman costs %d usec.\n", time_interval_pool/1000);
#if 0
		ptr = (char *)mem_alloc(sizeof(char));
		if (NULL == ptr)
		{
			log_error("Failed to mem_alloc.");
			break;
		}

		p_struct = (test_struct *)mem_alloc(sizeof(test_struct));
		if (NULL == p_struct)
		{
			log_error("Failed to mem_alloc.");
			break;
		}

		p_union = (test_union *)mem_alloc(sizeof(test_union));
		if (NULL == p_union)
		{
			log_error("Failed to mem_alloc.");
			break;
		}

		log_info("Test 0.1 >> Verify memory banks.");
		for (i = 0U; i < MAX_MEM_ALLOCATABLE_SIZE; ++i)
		{
			if(NULL != g_mem_pool.mem_banks[i].p_mem_available)
			{
				printf("Memory bank #%d allocated.\n", i);
			}
		}

		log_info("Memory allocated");

		log_info("Test 0.2 >> Free memory.");
		mem_free(ptr);
		mem_free(p_struct);
		mem_free(p_union);
#endif

		log_info("Memory all freed.");
		result = SYSTEM_STATUS_OK;
	}while(0);

	if (SYSTEM_STATUS_OK != result)
	{
		printf("Test Failed.\n");
	}

	printf("Test finished.\n");
}
