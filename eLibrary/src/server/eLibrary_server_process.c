/*
 * eLibrary_server_process.c
 *
 *  Created on: 5 Feb 2014
 *      Author: murphy
 */
#include "common_types.h"
#include "common_thread.h"
#include "common_handle.h"
#include "eLibrary_serverTypes.h"
#include "eLibrary_sharedTypes.h"

void thread_SVR_daemon(void)
{
	SYSTEM_STATUS result = COMMON_STATUS_OK;
	uint32_t event = 0U;
	uint32_t spec_size = 0U;
	void *job_spec = NULL;
	char buf[999U];
	USER_ID user_id = USER_ID_INVALID;
	COMMON_HANDLE session_handle = COMMON_NULL_HANDLE;
	char user_name[MAX_NAME_LENGTH] = {0U};
	char cmd[MAX_CMD_LENGTH];
	uint32_t cmd_length = 0U;
	RETRIEVAL_TYPE retrieval_type;
	COMMON_HANDLE session_id;
	RETRIEVAL_TYPE retrieval_type = RETRIEVAL_TYPE_invalid;
	SERVER_RESPONSE response_data;
	COMMON_HANDLE client_id;
	SERVER_SESSION *p_session_data = NULL;
	JOB_MSG_DATA msg_data;

	COMMON_DIAG_ENTRY("thread_SVR_daemon");

	while (COMMON_STATUS_OK == COMMON_QUEUE_pend(g_server_cxt.queue_id))
	{
		result = COMMON_QUEUE_getEvent(g_server_cxt.queue_id, &event);
		COMMON_DIAG_ASSERT_EQUAL(result, COMMON_STATUS_OK);

		if (SERVER_EVENT_TERMINATION == event)
		{
			for (i = 0U; i < MAX_NUM_WORKERS; i++)
			{
				result = COMMON_PNC_assignJob(g_server_cxt.job_center, 0U, NULL, 0U);
			}

			result = COMMON_QUEUE_destroyMsg(g_server_cxt.queue_id);
			break;
		}
		else
		{
			result = COMMON_QUEUE_getPayload(g_server_cxt.queue_id, &msg_data);
			COMMON_DIAG_ASSERT_EQUAL(result, COMMON_STATUS_OK);


			result = COMMON_PNC_assignJob(g_server_cxt.job_center, msg_data->retrieval_type, msg_data->pay_load, msg_data->conn_handle);

			result = COMMON_QUEUE_destroyMsg(g_server_cxt.queue_id);
		}
	}

	COMMON_DIAG_INFO("Server daemon terminating...");

	COMMON_DIAG_EXIT(result);
}

void thread_SVR_worker(void)
{
	uint8_t job_spec[MAX_JOB_SPEC_LENGTH];
	uint32_t event = 0U;
	uint32_t spec_size = 0U;
	uint32_t conn_handle = 0U;
	RETRIEVAL_TYPE retrieval_type;
	SERVER_RESPONSE response_data;
	SYSTEM_STATUS result;

	for (;;)
	{
		if (COMMON_STATUS_OK != COMMON_PNC_pickJob(g_server_cxt.job_center, &retrieval_type, job_spec, &conn_handle))
		{
			COMMON_DIAG_ERROR("COMMON_PNC_catachJob() failed.");
		}
		else if (RETRIEVAL_TYPE_WORKER_TERM != retrieval_type)
		{
			/*Get current_user_id from session*/

			memset(&response_data, 0x00, sizeof(SERVER_RESPONSE));
			response_data.retrieval_type = retrieval_type;

			switch (retrieval_type)
			{
				case RETRIEVAL_TYPE_CREATE_USER:
					eLibrary_SVR_Handler_AddUser(conn_handle, job_spec, &response_data);
				break;

				case RETRIEVAL_TYPE_BROWSE_BOOKS:
					eLibrary_SVR_Handler_BrowseBooks(conn_handle, job_spec, &response_data);
				break;

				case RETRIEVAL_TYPE_BROWSE_USERS:
					eLibrary_SVR_Handler_BrowseUsers(conn_handle, job_spec, &response_data);
				break;

				case RETRIEVAL_TYPE_ADD_FRIEND:
					eLibrary_SVR_Handler_AddFriend(conn_handle, job_spec &response_data);
				break;

				case RETRIEVAL_TYPE_RM_FRIEND:
					eLibrary_SVR_Handler_RmFriend(conn_handle, job_spec &response_data);
				break;

				case RETRIEVAL_TYPE_ADD_BOOK_DESP:
					eLibrary_SVR_Handler_AddBookDescription(conn_handle, job_spec &response_data);
				break;

				default:
				break;
			}

			result = g_server_cxt.conn.sendData(conn_handle, response_data);
		}
		else
		{
			break;
		}
	}
}

void CNM_event_listener(uint32_t conn_handle, uint32_t retrieval_type, void *pay_load)
{
	JOB_MSG_DATA msg_data;

	msg_data = malloc(sizeof(JOB_MSG_DATA));

	msg_data.conn_handle = conn_handle;
	msg_data.retrieval_type = retrieval_type;
	msg_data.pay_load = pay_load;

	COMMON_QUEUE_postMsg(g_server_cxt.queue_id, SERVER_EVENT_JOB, sizeof(JOB_MSG_DATA), msg_data);
}
