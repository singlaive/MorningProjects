/*
 * common_memory_pool.c
 *
 *  Created on: 31 Aug 2013
 *      Author: singlaive
 */

#include "common_types.h"
#include "common_memory_pool.h"

void *COMMON_POOL_malloc(COMMON_HANDLE memory_pool_handle, size_t size)
{
	void *ptr = NULL;

	if (MEMORY_POOL_NULL_HANDLE == memory_pool_handle)
	{
		ptr = malloc(size);
		if (NULL == ptr)
		{
			COMMON_DIAG_ASSERT_NOT_EQUAL(NULL, ptr);
		}
	}

	return ptr;
}

void COMMON_MEMPOOL_free(COMMON_HANDLE memory_pool_handle, void *ptr)
{
	if (MEMORY_POOL_NULL_HANDLE == memory_pool_handle)
	{
		free(ptr);
		ptr = NULL;
	}
}
