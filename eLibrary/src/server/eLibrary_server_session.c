/*
 * eLibrary_serverSession.c
 *
 *  Created on: 14 Aug 2013
 *      Author: singlaive
 */

#include "common_types.h"
#include "eLibrary_serverTypes.h"
#include "eLibrary_sharedTypes.h"

/* Session will be live when reading feature is enabled. */
#if 0
SYSTEM_STATUS eLibrary_SVR_sessionCreate(USER_ID user_id, COMMON_HANDLE *p_session_handle)
{
	SYSTEM_STATUS result = COMMON_STATUS_FAIL;
	SERVER_SESSION *p_session_data;
	COMMON_HANDLE l_handle = COMMON_NULL_HANDLE;

	COMMON_DIAG_ENTRY("eLibrary_server_sessionCreate");

	- = COMMON_malloc(sizeof(SERVER_SESSION));

	result = COMMON_HDL_handleCreate(p_session_data, p_session_handle)
	if (COMMON_STATUS_OK != result)
	{
		COMMON_DIAG_ERROR("COMMON_HDL_handleCreate() failed.");
		COMMON_free(p_session_data);
	}
	else
	{
		p_session_data->user_id = user_id;

		DIAG_INFO("New session %#x created for user %#x.", l_handle, user_id);
		*p_session_handle = l_handle;
	}

	COMMON_DIAG_EXIT(result);
	return result;
}

SYSTEM_STATUS eLibrary_SVR_sessionDestroy(COMMON_HANDLE session_handle)
{
	SYSTEM_STATUS result = COMMON_STATUS_FAIL;
	SERVER_SESSION *p_session_data = NULL;
	COMMON_DIAG_ENTRY("eLibrary_server_sessionDestroy");

	result = COMMON_HDL_getObject(session_handle, (void **)&p_session_data);
	if (COMMON_STATUS_OK != result)
	{
		COMMON_DIAG_ERROR("COMMON_HDL_handleCreate() failed.");
	}
	else
	{
		result = COMMON_HDL_handleDestroy(session_handle);
		if (COMMON_STATUS_OK != result)
		{
			COMMON_DIAG_ERROR("COMMON_HDL_handleCreate() failed.");
		}

		COMMON_free(p_session_data);
	}

	COMMON_DIAG_EXIT(result);
	return result;
}
#endif
