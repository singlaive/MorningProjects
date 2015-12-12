/*
 * eLibrary_server.c
 *
 *  Created on: 1 Feb 2013
 *      Author: singlaive
 */

#include "common_types.h"
#include "common_thread.h"
#include "common_handle.h"
#include "eLibrary_serverTypes.h"
#include "eLibrary_sharedTypes.h"
#include "eLibrary_server_process.h"
#include "eLibrary_CNM_API.h"
#include "eLibrary_DB_API.h"

SYSTEM_STATUS eLibrary_SVR_API_init(CNM_INTERFACE cnm_interfaces, DB_ENGINE db_interfaces)
{
	SYSTEM_STATUS result = COMMON_STATUS_FAIL;

	g_server_cxt.conn = cnm_interfaces;
	g_server_cxt.db = db_interfaces;

	COMMON_DIAG_ENTRY("eLibrary_serverInit");

	/* We might want do this in different way */
	// g_server_cxt.conn.addPlugin();

	result = g_server_cxt.conn.registerEventCB(CNM_event_listener);

	result = COMMON_threadCreate(thread_SVR_daemon, NULL);

	for (i = 0U; i < MAX_NUM_WORKERS; i++)
	{
		result = COMMON_threadCreate(thread_SVR_worker, NULL);
	}

	COMMON_STATUS_EXIT(result);
	return result;
}

SYSTEM_STATUS eLibrary_SVR_API_term()
{
	SYSTEM_STATUS result = COMMON_STATUS_FAIL;
	COMMON_DIAG_ENTRY("eLibrary_serverTerm");

	//g_server_cxt.conn.rmPlugin();

	COMMON_QUEUE_postMsg(g_server_cxt.queue_id, SERVER_EVENT_TERMINATION, 0U, NULL);
	pthread_join(thread_id, NULL);

	COMMON_DIAG_ENTRY("eLibrary_serverInit");
	return result;
}
