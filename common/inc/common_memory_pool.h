/*
 * common_memory_pool.h
 *
 *  Created on: 31 Aug 2013
 *      Author: singlaive
 */

#ifndef COMMON_MEMORY_POOL_H_
#define COMMON_MEMORY_POOL_H_

#define COMMON_MEMPOOL_NULL_HANDLE 0U

void *COMMON_MEMPOOL_malloc(COMMON_HANDLE memory_pool_handle, size_t size);

void COMMON_MEMPOOL_free(COMMON_HANDLE memory_pool_handle, void *ptr);

#endif /* COMMON_MEMORY_POOL_H_ */
