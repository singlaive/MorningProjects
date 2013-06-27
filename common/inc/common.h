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
#include <errno.h>
#include <time.h>
#include <string.h>

#define NULL_CHAR '\0'

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
#define MAX_CMD_STR_LENGTH  128U
#define MAX_NUM_POSIX_ERROR 200U /* Maximal num of errno specified in POSIX. This no is dinfined by myself. */

#define DEBUG_MODE /* This does not work, yet. */

#if (defined DEBUG_MODE)
#define DIAG_INFO(msg, arg...)	printf("[%s]%d INFO  %s() %d: " msg "\n", myAscTime(), (uint32_t)time(NULL), __FILE__, __LINE__, ##arg);
#define DIAG_PARAM(msg, arg...)	printf("[%s]%d PARAM %s() %d: " msg "\n", myAscTime(), (uint32_t)time(NULL), __FILE__, __LINE__, ##arg);
#define DIAG_ERROR(msg, arg...)	printf("[%s]%d ERROR %s() %d: " msg "\n", myAscTime(), (uint32_t)time(NULL), __FILE__, __LINE__, ##arg);
#define DIAG_WARN(msg, arg...)	printf("[%s]%d WARN  %s() %d: " msg "\n", myAscTime(), (uint32_t)time(NULL), __FILE__, __LINE__, ##arg);
#define DIAG_FATAL(msg, arg...) do{printf("[%s]%d FATAL %s() %d: " msg, myAscTime(), (uint32_t)time(NULL), __FILE__, __LINE__, ##arg); printf("(%s)\n", strerror(errno));}while(0);
#else
#define DIAG_INFO(msg, arg...)
#define DIAG_PARAM(msg, arg...)
#define DIAG_ERROR(msg, arg...)
#define DIAG_WARN(msg, arg...)myAscTime(), time(NULL),
#define DIAG_FATAL(msg, arg...)
#endif

/*do{uint32_t i; 																			\
	for (i = 0; i < MAX_NUM_POSIX_ERROR; i++){ 																\
		if (errInfo[i].err == errno){ 																				\
			break; 																										\
		} 																												\
	}																													\
	printf("FATAL %s() %d: " msg "[errno=%d;%s;%s]\n", __FILE__, __LINE__, ##arg, errInfo[i].errno, errInfo[i].errCode, errInfo[i].errStr);	\
}while(0)*/

struct a
{
	uint32_t err;
	char * errCode;
	char * errStr;
} errInfo[MAX_NUM_POSIX_ERROR];

/********************************************************************/
/* 						Structs, Unions, etc.						*/
/********************************************************************/

typedef uint32_t SYSTEM_STATUS;

void *p_trees[MAX_OBJECTS];
void *p_hashtables[MAX_OBJECTS];

typedef struct cmd
{
	char str[MAX_CMD_STR_LENGTH]; /* Original command string. */
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
char * myAscTime(void);
char * myAscTimeLite(time_t * time_sec);
SYSTEM_STATUS getAscTime(char *str);

void COMMON_free(void * p);

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
