/*
 * mem_common.c
 *
 *  Created on: 2011-11-24
 *      Author: singlaive
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mem_common.h"

uint32_t time_diff(struct timeval time_start, struct timeval time_end)
{
	uint32_t result;

	result = (time_end.tv_sec - time_start.tv_sec)*1000000 + (time_end.tv_usec - time_start.tv_usec);

	return result;
}
