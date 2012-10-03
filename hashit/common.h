/*
 * common.h
 *
 *  Created on: 2012-3-11
 *      Author: singlaive
 */

/* Modification history:
 *     11th Apr 2012 Tree implementation
 *     6th May 2012 Hash table implementation
 *
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdbool.h>

#define STATUS_OK		0x00000000
#define STATUS_FAIL		0x00000001
#define STATUS_INVALID_PARAM 0x00000002
#define STATUS_DATABASE_FULL 0x00000003
#define STATUS_DATABASE_NOT_FOUND 0x00000004
#define STATUS_DATABASE_DUPLICATED 0x00000005
#define STATUS_DATABASE_OVERWRITEN 0x00000006

#define HANDLE_BASE_TREE	    0x10000000 /* Base value for generating a handle for tree appliance. */
#define HANDLE_BASE_HASH_TABLE	0x01000000 /* Base value for generating a handle for hash table appliance. */
#define HANDLE_COUNTER_MASK 	0x000000FF /* Mask to get the index to the data array. */

#define MAX_OBJECTS			128U /* Maximal num of objects for each type of data. For example, there are such no of trees that can be created. */
#define MAX_CMD_LENGTH		12U

#define DIAG_INFO(msg, arg...)	printf("INFO %s() %d: " msg "\n", __FUNCTION__, __LINE__, ##arg);
#define DIAG_PARAM(msg, arg...)	printf("PARAM %s() %d: " msg "\n", __FUNCTION__, __LINE__, ##arg);
#define DIAG_ERROR(msg, arg...)	printf("ERROR %s() %d: " msg "\n", __FUNCTION__, __LINE__, ##arg);
#define DIAG_WARN(msg, arg...)	printf("WARN %s() %d: " msg "\n", __FUNCTION__, __LINE__, ##arg);

/********************************************************************/
/* 						Structs, Unions, etc.						*/
/********************************************************************/

typedef uint32_t SYSTEM_STATUS;

void *p_trees[MAX_OBJECTS];
void *p_hashtables[MAX_OBJECTS];

typedef struct cmd
{
	char cmd[MAX_CMD_LENGTH];
	uint32_t num_options;
	char *option; 			/* Array of options. */
	uint32_t num_params;
	char **params;			/* Array of params. No limitation about length of each parameter. */
}cmd_line_tag;

/********************************************************************/
/* 							Func prototypes							*/
/********************************************************************/

void fgets_convert(char * str);

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

/* Format the commend line input
 * .
 * str [in] string to be parsed. It must not be stored in static address.
 * p_commend_line [out] parsed commends.
 **/
SYSTEM_STATUS parse_cmd(char *str, cmd_line_tag *p_commend_line);

#endif /* COMMON_H_ */
