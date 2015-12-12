/*
 * common_thread.c
 *
 *  Created on: 12 Aug 2013
 *      Author: singlaive
 */

#include "common.h"

SYSTEM_STATUS COMMON_threadCreate(void thread_routine, void * arg)
{
	SYSTEM_STATUS result = STATUS_FAIL;
	pthread_t thread_id;

	COMMON_DIAG_ENTRY("COMMON_threadCreate");

	result = pthread_create (&thread_id, NULL, thread_routine, arg);
	if (0 != result)
	{
		DIAG_ERROR("Fail to create thread.");
		break;
	}

	/* Set joinable and detchable and so on. */

	COMMON_DIAG_EXIT(result);
	return result;
}
