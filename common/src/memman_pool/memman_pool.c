/*
 ============================================================================
 Name        : memman_pool.c
 Author      : Murphy Meng
 Version     :
 Copyright   : Free
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memman_pool.h"
#include "mem_common.h"

int g_mem_node_size = 10;

SYSTEM_STATUS mem_pool_init(void)
{
	SYSTEM_STATUS result = SYSTEM_STATUS_OK;

	memset(&g_mem_pool, 0, sizeof(g_mem_pool));

	return result;
}

SYSTEM_STATUS mem_pool_destroy(void)
{
	SYSTEM_STATUS result = SYSTEM_STATUS_OK;
	uint32_t i;
	mem_bank_t * p_mem_bank = NULL;

	for (i = 0U; i < MAX_MEM_ALLOCATABLE_SIZE; ++i)
	{
		p_mem_bank = &(g_mem_pool.mem_banks[i]);
		if (NULL != p_mem_bank->p_mem_chunk_list)
		{
			mem_chunk_t * p_current_mem_chunk = p_mem_bank->p_mem_chunk_tail;
			mem_chunk_t * p_mem_chunk_node = NULL;

			while(NULL != p_current_mem_chunk)
			{
				p_mem_chunk_node = p_current_mem_chunk;
				p_current_mem_chunk = p_current_mem_chunk->previous_chunk;
				free(p_mem_chunk_node->p_mem_chunk);
				free(p_mem_chunk_node);
			}

			p_mem_bank->p_mem_available = NULL;
			p_mem_bank->p_mem_chunk_list = NULL;
			p_mem_bank->p_mem_chunk_tail = NULL;
		}
	}

	return result;
}

SYSTEM_STATUS mem_bank_init(uint8_t index)
{
	uint8_t mem_unit_size = 0U;
	mem_bank_t * p_mem_bank = NULL;
	mem_chunk_t * p_mem_chunk_node;

	/* We are going to create a memory bank handling memory apply/free of fixed
	 * size of mem_unit_size. */
	mem_unit_size = index;
	p_mem_bank = &(g_mem_pool.mem_banks[index]);

	p_mem_chunk_node = (mem_chunk_t *)malloc(sizeof(mem_chunk_t));
	if (NULL == p_mem_chunk_node)
	{
		log_error("Malloc failed.");
		return SYSTEM_STATUS_FAIL;
	}

	p_mem_bank->p_mem_chunk_list = p_mem_chunk_node;
	p_mem_bank->p_mem_chunk_tail = p_mem_chunk_node;

	p_mem_bank->p_mem_chunk_list->p_mem_chunk = (mem_node_t *)malloc(NUM_MEM_NODES_PER_BANK*mem_unit_size);
	if (NULL == p_mem_bank->p_mem_chunk_list->p_mem_chunk)
	{
		log_error("Malloc failed.");
		return SYSTEM_STATUS_FAIL;
		free(p_mem_chunk_node);
	}

	p_mem_bank->p_mem_chunk_list->next_chunk = NULL;
	p_mem_bank->p_mem_chunk_list->previous_chunk = NULL;

	p_mem_bank->p_mem_available = p_mem_bank->p_mem_chunk_list->p_mem_chunk;

	mem_list_format(p_mem_bank->p_mem_available, mem_unit_size);

	return SYSTEM_STATUS_OK;
}

SYSTEM_STATUS mem_add_bank(uint8_t index)
{
	SYSTEM_STATUS result = SYSTEM_STATUS_FAIL;

	mem_bank_t * p_mem_bank;

	p_mem_bank = &(g_mem_pool.mem_banks[index]);
	if (NULL == p_mem_bank->p_mem_chunk_list)
	{
		result = mem_bank_init(index);
		if (SYSTEM_STATUS_OK != result)
		{
			log_error("Failed to mem_bank_init");
		}
		else
		{
			result = SYSTEM_STATUS_OK;
		}
	}
	else
	{
		uint8_t mem_unit_size = index;
		mem_chunk_t * p_mem_chunk_node = NULL;

/*		printf("Meng: scale of current chunk %d\n", (p_mem_bank->p_mem_available - (mem_node_t *)p_mem_bank->p_mem_chunk_list));
*/
		p_mem_chunk_node = (mem_chunk_t *)malloc(sizeof(mem_chunk_t *));
		p_mem_chunk_node->p_mem_chunk = (mem_node_t *)malloc(NUM_MEM_NODES_PER_BANK*mem_unit_size);
		p_mem_chunk_node->next_chunk = NULL;
		p_mem_chunk_node->previous_chunk = p_mem_bank->p_mem_chunk_tail;

		p_mem_bank->p_mem_chunk_tail->next_chunk = p_mem_chunk_node;
		p_mem_bank->p_mem_chunk_tail = p_mem_chunk_node;
		p_mem_bank->p_mem_available = p_mem_chunk_node->p_mem_chunk;
		mem_list_format(p_mem_chunk_node->p_mem_chunk, mem_unit_size);

		result = SYSTEM_STATUS_OK;
	}

	return result;
}

void mem_list_format(mem_node_t * p_mem_list_head,
					 size_t		  size)
{
	uint32_t i, step;
	mem_node_t * p_mem_node = NULL;

	memset(p_mem_list_head, 0x00, size*NUM_MEM_NODES_PER_BANK);

	step = size;
	p_mem_node = p_mem_list_head;

	for (i = 1U; i < NUM_MEM_NODES_PER_BANK; ++i)
	{
		/* The next node should be "step" bytes away. For any size required
		 * smaller than 4bytes, i.e. sizeof(mem_node_t), always allocated 4
		 * bytes. */
		p_mem_node->next_node = (mem_node_t *)((char *)p_mem_node + step);
		p_mem_node = p_mem_node->next_node;
	}


}

void * mem_alloc(size_t size)
{
	void * result;

	/* Them size to be allocated will be one bype more. The extra byte will be
	 * used to let us know the size to be allocated. */
	++size;

	/* For big chunk of memory, use malloc to do the job. */
	if (size > MAX_MEM_ALLOCATABLE_SIZE)
	{
		result= malloc(size);
		if (NULL == result)
		{
			log_error("Malloc failed to allocate bytes.");
		}
		else
		{
			log_info("Allocate bytes using malloc.");
		}
	}
	else /* For small pieces, use our pool. */
	{
		/* The minimum memory allocated will be 4bytes, i.e. sizeof(mem_node_t). */
		uint8_t index = (uint8_t)(size > 4U)?size:4U;
		mem_node_t ** p_free_mem_head;
		mem_node_t * p_available_mem_node = NULL;

		p_free_mem_head = &(g_mem_pool.mem_banks[index].p_mem_available);

		/* When the appropriate memory bank (the bank[index] one) has not been
		 * allocated from the physical; or it has been full, apply a new piece
		 * of memory for this bank. */
		if (NULL == *p_free_mem_head)
		{
			if (SYSTEM_STATUS_OK != mem_add_bank(index))
			{
				log_error(("Failed to mem_bank_init."));
				return NULL;
			}
		}

		/* Move the free memory head to the next available node. */
		p_available_mem_node = *p_free_mem_head;
		*p_free_mem_head = (*p_free_mem_head)->next_node;

		/* Write the size of the memory bit to be allocated and return the pointer to it. */
		*(uint8_t *)p_available_mem_node = (uint8_t)size;
		result = (void *)((uint8_t *)p_available_mem_node + 1);
	}

	return result;
}

void mem_free(void * p_membit)
{
	uint8_t index;
	mem_bank_t * p_mem_bank;
	mem_node_t * p_mem_node;

	/* Move one byte backwards to get the size of this bit of memory, which
	 * also helps locate the correct memory bank in the memory pool.  */
	p_mem_node = (mem_node_t *)((uint8_t *)p_membit-1U);
	index = (uint8_t)p_mem_node->buffer;
	p_mem_bank = &(g_mem_pool.mem_banks[index]);
	p_mem_node->next_node = p_mem_bank->p_mem_available;
	p_mem_bank->p_mem_available = p_mem_node;
}
