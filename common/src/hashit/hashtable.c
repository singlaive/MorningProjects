/*
 ============================================================================
 Name        : hashit.c
 Author      : Murphy Meng
 Version     :
 Copyright   : Murphy Meng 2012
 Description : Hash table implementation
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/inc/hashtable.h"
#include "common.h"

void *ht_lookup(hashtable_tag *p_ht, void *p_key, uint32_t *p_probe_count, int32_t *p_location);

SYSTEM_STATUS HT_create(uint32_t (*hash)(void *key),
					    int32_t (*compare)(void *key1, void *key2),
					    void (*display)(void *key, void *data),
					    void (*remove)(void *key),
					    uint8_t *p_HT_handle)
{
	SYSTEM_STATUS status = STATUS_FAIL;
	hashtable_tag *p_ht;

	if ((NULL == hash) || (NULL == compare) || (NULL == remove) || (NULL == display))
	{
		DIAG_ERROR("Invalid callbacks.");
	}
	else
	{
		p_ht = malloc(sizeof(hashtable_tag));
		if (NULL != p_ht)
		{
			(void)COMMON_handle_create((void *)p_ht, p_HT_handle);

			p_ht->compare = compare;
			p_ht->hash = hash;
			p_ht->display = display;
			p_ht->remove = remove;
			p_ht->load_factor = 0;
			memset(p_ht->ht, 0x00, NUM_BUCKETS*sizeof(ht_bucket_tag));

			status = EXIT_SUCCESS;
		}
		else
		{
			DIAG_ERROR("malloc failed.");
		}
	}

	return status;
}

SYSTEM_STATUS HT_destroy(uint8_t HT_handle)
{
	SYSTEM_STATUS status = STATUS_OK;
	hashtable_tag *p_ht = NULL;
	uint32_t i;

	(void)COMMON_object_retrieve(HT_handle, (void **)&p_ht);

	for(i = 0U; i < NUM_BUCKETS; i++)
	{
		if (NULL != p_ht->ht[i].p_data)
		{
			p_ht->remove(p_ht->ht[i].p_data);
		}
	}

	return status;
}

SYSTEM_STATUS HT_insert(uint8_t HT_handle,
						void *p_key,
				  	    void *p_data,
				  	    uint32_t *p_probe_count,
				  	    int32_t *p_location)
{
	SYSTEM_STATUS status = STATUS_FAIL;
	hashtable_tag *p_ht = NULL;
	ht_bucket_tag *p_bucket = NULL;

	(void)COMMON_object_retrieve(HT_handle, (void **)&p_ht);

	if (p_ht->num_data == NUM_BUCKETS)
	{
		DIAG_ERROR("Hash table full.");
		status = STATUS_DATABASE_FULL;
	}
	else
	{
		p_bucket = ht_lookup(p_ht, p_key, p_probe_count, p_location);
		if (*p_probe_count == 100*NUM_BUCKETS)
		{
			status = STATUS_DATABASE_NOT_FOUND;
		}
		else if ((NULL == p_bucket->p_data) || (true == p_bucket->vacated) ||
				((NULL != p_bucket->p_data) && (0 == p_ht->compare(p_bucket->p_key, p_key))))
		{
			if (NULL == p_bucket->p_data)
			{
				status = STATUS_OK;
			}
			else
			{
				status = STATUS_DATABASE_OVERWRITEN;
			}

			p_bucket->p_data = p_data;
			p_bucket->p_key = p_key;
			p_bucket->vacated = false;

			p_ht->num_data++;
			p_ht->load_factor = (float)p_ht->num_data / (float)NUM_BUCKETS;

		}
		else
		{
			status = STATUS_DATABASE_DUPLICATED;
		}
	}

	return status;
}

SYSTEM_STATUS HT_lookup(uint8_t HT_handle,
						void *p_key,
						void **p_data,
						uint32_t *p_probe_count,
						int32_t *p_location)
{
	SYSTEM_STATUS status = STATUS_OK;
	hashtable_tag *p_ht = NULL;
	ht_bucket_tag *p_bucket = NULL;

	(void)COMMON_object_retrieve(HT_handle, (void **)&p_ht);

	p_bucket = ht_lookup(p_ht, p_key, p_probe_count, p_location);
	if ((NULL == p_bucket->p_data) || (true == p_bucket->vacated))
	{
		*p_location = -1;
		status = STATUS_DATABASE_NOT_FOUND;
	}
	else
	{
		*p_data = p_bucket->p_data;
	}

	return status;
}

SYSTEM_STATUS HT_remove(uint8_t HT_handle,
						void *p_key,
						uint32_t *p_probe_count,
						int32_t *p_location)
{
	SYSTEM_STATUS status = STATUS_FAIL;
	hashtable_tag *p_ht = NULL;
	ht_bucket_tag *p_bucket = NULL;

	(void)COMMON_object_retrieve(HT_handle, (void **)&p_ht);

	p_bucket = ht_lookup(p_ht, p_key, p_probe_count, p_location);
	if ((NULL == p_bucket->p_data) || (true == p_bucket->vacated))
	{
		*p_location = -1;
		status = STATUS_DATABASE_NOT_FOUND;
	}
	else
	{
		p_ht->remove(p_bucket->p_data);
		p_bucket->p_data = NULL;
		p_bucket->p_key = NULL;
		p_bucket->vacated = true;

		p_ht->num_data--;
		p_ht->load_factor = (float)p_ht->num_data / (float)NUM_BUCKETS;
		status = STATUS_OK;
	}

	return status;
}

/* Given the key, it provides a pointer of the result in the hash table.
 * If the data exits, the returned pointer contains its address.
 * If the data does not exit, the returned pointer either returns NULL
 * or a vacated bucket address. */
void *ht_lookup(hashtable_tag *p_ht,
				void *p_key,
				uint32_t *p_probe_count,
				int32_t *p_location)
{
	uint32_t index = 0U;
	ht_bucket_tag *p_vacated_bucket = NULL;
	bool dublicated_found = false;

	*p_probe_count = 0U;
	srand(NUM_BUCKETS/2);
	index = p_ht->hash(p_key);

	/* The reason make the max i being num_data is, the condition of
	 * breaking out the loop can be limited to either NULL (no matching)
	 * or a valid bucket (matching). If the max i is num_data-1, in the
	 * worst case, the loop shall stop at the last position which contains
	 * a valid bucket and it is not what we want.
	 * Doing so makes choosing return value easy.
	 * Check before entering this func ensures num_data is smaller than
	 * the size of hash table. */
	while ((NULL != p_ht->ht[index].p_data) || (true == p_ht->ht[index].vacated))
	{
		if (true == p_ht->ht[index].vacated)
		{
			/* Only return the first vacated bucket if needed. */
			if (NULL == p_vacated_bucket)
			{
				p_vacated_bucket = &(p_ht->ht[index]);
				*p_location = index;
			}
		}
		else if ((NULL != p_ht->ht[index].p_data) && (0 == p_ht->compare(p_ht->ht[index].p_key, p_key)))
		{
			dublicated_found = true;
			break;
		}
		else
		{
			/* Do nothing. */
		}

		if (*p_probe_count == 100*NUM_BUCKETS)
		{
			DIAG_WARN("Can not locate available bucket. Bad algorithm.")
			return NULL;
		}

		index = (index + rand()) % NUM_BUCKETS;
		(*p_probe_count)++;
	}

	if (true == dublicated_found)
	{
		*p_location = index;
		return &(p_ht->ht[index]);
	}
	else if (NULL != p_vacated_bucket)
	{
		return p_vacated_bucket;
	}
	else if (NULL == p_ht->ht[index].p_data)
	{
		*p_location = index;
		return &(p_ht->ht[index]);
	}
	else
	{}
}

SYSTEM_STATUS HT_traverse(uint8_t HT_handle)
{
	SYSTEM_STATUS status = STATUS_OK;
	hashtable_tag *p_ht = NULL;
	uint32_t i;

	(void)COMMON_object_retrieve(HT_handle, (void **)&p_ht);

	printf("=====Hash Table=====\n");
	printf("Total entries %d.\n", p_ht->num_data);
	printf("Load factor %3.2f.\n", p_ht->load_factor);
	for(i = 0U; i < NUM_BUCKETS; i++)
	{
		printf("Bucket #%d = ", i);
		if (NULL != p_ht->ht[i].p_data)
		{
			p_ht->display(p_ht->ht[i].p_key, p_ht->ht[i].p_data);
			printf("\n");
		}
		else if (true == p_ht->ht[i].vacated)
		{
			printf("\tDeleted\n");
		}
		else
		{
			printf("\tNULL\n");
		}
	}
	printf("=====Hash Table=====\n");

	return status;
}

