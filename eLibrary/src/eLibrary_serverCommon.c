/*
 * eLibrary_serverCommon.c
 *
 *  Created on: 2013-2-9
 *      Author: singlaive
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "eLibrary_serverTypes.h"
#include "eLibrary_serverCommon.h"
#include "eLibrary_serverHandler.h"

pthread_t thread_id;

SYSTEM_STATUS eLibrary_serverInit()
{
	SYSTEM_STATUS result = STATUS_FAIL;


	do
	{
		g_lock_exchange_count = 0U;
		g_unlock_exchange_count = 0U;

		g_msgExchange_ready = false;
		DIAG_INFO("g_systemEvent = %d", g_systemEvent);
		result = pthread_create (&thread_id, NULL, thread_processMsg, NULL);
		if (0 != result)
		{
			DIAG_ERROR("Fail to create thread.");
			break;
		}

		memset(&g_userData, 0x00, sizeof(struct user_dataBase));

		if (0 != pthread_mutex_init(&g_userData.lock, NULL))
		{
			DIAG_ERROR("pthread_mutex_init() failed.");
			break;
		}

		result = STATUS_OK;
	}while(0);

	return result;
}

SYSTEM_STATUS eLibrary_serverTerm()
{
	SYSTEM_STATUS result = STATUS_FAIL;

	do
	{

		(void)pthread_mutex_lock(&lock_exchange);
		g_lock_exchange_count++;
		DIAG_INFO("g_lock_exchange_count = %d", g_lock_exchange_count);

		g_systemEvent = SYSTEM_EVENT_SHUTDOWN;
		(void)pthread_cond_signal(&cond_exchange);
		(void)pthread_mutex_unlock(&lock_exchange);

		g_unlock_exchange_count++;
		DIAG_INFO("g_unlock_exchange_count = %d", g_unlock_exchange_count);

		pthread_join(thread_id, NULL);
		result = STATUS_OK;
	}while(0);

	return result;
}
