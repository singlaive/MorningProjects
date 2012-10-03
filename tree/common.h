/*
 * common.h
 *
 *  Created on: 2012-3-11
 *      Author: singlaive
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdbool.h>

#define HANDLE_BASE_TREE	0x10000000 /* Base value for generating a handle for tree appliance. */
#define HANDLE_COUNTER_MASK 0x000000FF /* Mask to get the index to the data array. */

#define MAX_OBJECTS			128U /* Maximal num of objects for each type of data. For example, there are such no of trees that can be created. */

void *p_trees[MAX_OBJECTS];

void fgets_convert(char * str, char * str1);

/* Make up a integer handle for any given element.
 * ptr 		[in] pointer referencing a user defined data
 * p_handle [out] handle indicating the data.
 */
int COMMON_handle_create(void *ptr, uint8_t *p_handle);

/* Retrieve the address to the user data indicated by a given handle.
 * handle [in] handle indicating a data
 * ptr	  [out] the user data
 **/
int COMMON_object_retrieve(uint8_t handle, void **ptr);


#endif /* COMMON_H_ */
