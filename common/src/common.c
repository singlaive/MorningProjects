/*
 * common.c
 *
 *  Created on: 2012-3-11
 *      Author: singlaive
 */

/* Modification history:
*     11th Apr 2012 Tree implementation
 *     6th May 2012 Hash table implementation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

char * myAscTime(void);

void fgets_convert(char * str)
{
	char * ptr = str;

	while (('\n' != *ptr) && ('\0' != *ptr))
	{
		ptr++;
	}

	if ('\n' == *ptr)
	{
		*ptr = '\0';
	}
}

int COMMON_handle_create(void *ptr, uint8_t *p_handle)
{
	static uint8_t counter = 0U;

	p_trees[counter] = ptr;
	*p_handle = HANDLE_BASE_TREE | counter;
	counter++;

	return EXIT_SUCCESS;
}

int COMMON_object_retrieve(uint8_t handle, void **ptr)
{
	uint32_t index = handle & HANDLE_COUNTER_MASK;
	*ptr = p_trees[index];

	return EXIT_SUCCESS;
}

char * myAscTime(void)
{
    static char mon_name[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static char result[20];
    struct tm *timeptr;
    time_t time_sec = time(NULL);

    timeptr = localtime(&time_sec);
    sprintf(result, "%.3s%3d %.2d:%.2d:%.2d %d",
        mon_name[timeptr->tm_mon],
        timeptr->tm_mday, timeptr->tm_hour,
        timeptr->tm_min, timeptr->tm_sec,
        1900 + timeptr->tm_year);
    return result;
}

char * myAscTimeLite(time_t * time_sec)
{
    static char result[11];
    struct tm *timeptr;

    timeptr = localtime(time_sec);
    sprintf(result, "%2d-%2d-%4d", timeptr->tm_mday, timeptr->tm_mon, 1900 + timeptr->tm_year);
    return result;
}

/* The input must be sufficient large to hold 20 characters. */
SYSTEM_STATUS getAscTime(char *str)
{
	time_t time_sec = time(NULL);
	struct tm *timeptr;
	SYSTEM_STATUS result = STATUS_FAIL;

	char monName[12][3] = {
	        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

	if (NULL == str)
	{
		DIAG_ERROR("Invalid parameter");
		result = STATUS_FAIL;
	}
	else
	{
	    timeptr = localtime(&time_sec);
	    sprintf(str, "%.3s%3d %.2d:%.2d:%.2d %d",
	        monName[timeptr->tm_mon],
	        timeptr->tm_mday, timeptr->tm_hour,
	        timeptr->tm_min, timeptr->tm_sec,
	        1900 + timeptr->tm_year);
	}

	return result;
}

void COMMON_free(void * p)
{
	if (NULL != p)
		free(p);
}
