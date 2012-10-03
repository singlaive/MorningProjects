/*
 * mem_common.h
 *
 *  Created on: 2011-11-13
 *      Author: singlaive
 */

#ifndef MEM_COMMON_H_
#define MEM_COMMON_H_

#define log_error(msg) do{printf("Error: "); printf(msg); printf("\n");}while(0)
#define log_info(msg) do{printf("Info: "); printf(msg); printf("\n");}while(0)
#define log_warn(msg) do{printf("Warn: "); printf(msg); printf("\n");}while(0)

typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

typedef enum
{
	SYSTEM_STATUS_UNDEFINDE,
	SYSTEM_STATUS_OK,
	SYSTEM_STATUS_FAIL,
	SYSTEM_STATUS_OVER
}SYSTEM_STATUS;

uint32_t time_diff(struct timeval time_start, struct timeval time_end);
#endif /* MEM_COMMON_H_ */
