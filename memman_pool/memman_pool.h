/*
 * memman_pool.h
 *
 *  Created on: 2011-11-12
 *      Author: singlaive
 */

#include "mem_common.h"

#ifndef MEMMAN_POOL_H_
#define MEMMAN_POOL_H_

#define MAX_MEM_ALLOCATABLE_SIZE 128U
#define NUM_MEM_NODES_PER_BANK   10U

typedef union _mem_node
{
	union _mem_node * next_node;
	char  buffer;
}mem_node_t;

typedef struct _mem_chunk
{
	struct _mem_chunk * previous_chunk;
	struct _mem_chunk * next_chunk;
	void *  p_mem_chunk;
}mem_chunk_t;

typedef struct
{
	mem_chunk_t * p_mem_chunk_tail;
	mem_chunk_t * p_mem_chunk_list; /* Pointers to every single memory chunk obtained from the system. Will only be used to free them*/
	mem_node_t *  p_mem_available;
}mem_bank_t;

typedef struct
{
	mem_bank_t mem_banks[MAX_MEM_ALLOCATABLE_SIZE];

}mem_pool_t;

mem_pool_t g_mem_pool;

SYSTEM_STATUS mem_pool_init(void);
SYSTEM_STATUS mem_pool_destroy(void);
SYSTEM_STATUS mem_bank_init(uint8_t index);
SYSTEM_STATUS mem_add_bank(uint8_t index);
void mem_list_format(mem_node_t * p_mem_list_head, size_t size);
void * mem_alloc(size_t size);
void mem_free(void * p_membit);



#endif /* MEMMAN_POOL_H_ */
