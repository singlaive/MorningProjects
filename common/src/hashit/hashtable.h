/*
 * hashtable.h
 *
 *  Created on: 2012-5-6
 *      Author: singlaive
 */

/*
 * HT Hash table
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "common.h"

#define NUM_BUCKETS 20U

/********************************************************************/
/* 						Structs, Unions, etc.						*/
/********************************************************************/

typedef struct ht_bucket
{
	void *p_key;  /* Key of the user data. */
	void *p_data; /* Pointer referencing user data*/
	bool vacated; /* Flag indicating if there was data but removed. By default it is false. */
}ht_bucket_tag;

typedef struct hashtable
{
	uint32_t num_data;
	float    load_factor;
	ht_bucket_tag ht[NUM_BUCKETS];
	uint32_t (*hash)(void *key);
	int32_t (*compare)(void *key1, void *key2);
	void (*display)(void *key, void *data);
	void (*remove)(void *key);
}hashtable_tag;

/********************************************************************/
/* 							Func prototypes							*/
/********************************************************************/

/* Description: Create a hash table
 *
 * [in] remove  callback to remove the entry
 * [out] p_HT_handle handle of the hash table produced
 */
SYSTEM_STATUS HT_create(uint32_t (*hash)(void *key),
						int32_t (*compare)(void *key1, void *key2),
						void (*display)(void *key, void *data),
						void (*remove)(void *key),
						uint8_t *p_HT_handle);

/* Description: Insert an user defined data into a hash table
 *
 * [in] HT_handle  handle of the hash table
 * [in] p_key key to be hashed used to determine the position in the hash table for insertion
 * [in] p_data pointer to the data to be inserted
 *
 * rt: STATUS_OK: OK
 * 	   STATUS_FAIL: failure
 * 	   STATUS_DATABASE_FULL: Hash table full.
 */
SYSTEM_STATUS HT_insert(uint8_t HT_handle,
						void *p_key,
				  	    void *p_data,
				  	    uint32_t *p_probe_count,
				  	    int32_t *p_location);

/* Description: Remove an entry off a hash table.
 *
 * [in] HT_handle  handle of the hash table
 * [in] p_key key to be hashed used to determine the position in the hash table for removal
 *
 * rt: STATUS_OK: OK
 * 	   STATUS_FAIL: failure
 */
SYSTEM_STATUS HT_remove(uint8_t HT_handle,
		  	  	  	    void *p_key,
		  	  	  	    uint32_t *p_probe_count,
		  	  	  	    int32_t *p_location);

/* Description: Locate an entry from a hash table
 *
 * [in] HT_handle  handle of the hash table
 * [in] p_key key to be hashed used to determine the position in the hash table
 * [out] p_data pointer to the address of the entry
 */
SYSTEM_STATUS HT_lookup(uint8_t HT_handle,
		  	  	  	    void *p_key,
		  	  	  	    void **p_data,
		  	  	  	    uint32_t *p_probe_count,
		  	  	  	    int32_t *p_location);

/* Description: Create a hash table
 *
 * [in] HT_handle  handle of the hash table
 */
SYSTEM_STATUS HT_traverse(uint8_t HT_handle);

/* Description: Destroy the hash table
 *
 * [in] HT_handle  handle of the hash table
 */
SYSTEM_STATUS HT_destroy(uint8_t HT_handle);

#endif /* HASHIT_H_ */
