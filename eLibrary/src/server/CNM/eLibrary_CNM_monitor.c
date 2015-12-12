/*
 * eLibrary_CNM_monitor.c
 *
 *  Created on: 6 Feb 2014
 *      Author: murphy
 */

#include "common_types.h"
#include "eLibrary_CNM_API.h"
#include "eLibrary_CNM_monitor.h"
#include "eLibrary_CNM_types.h"


typedef struct
{
	COMMON_HANDLE conn_handle;

}CONN_INFO;
CNM_monitor_openConn(char *p_username, char *p_password, COMMON_HANDLE *p_conn_handle)
{
	SYSTEM_STATUS result;
	CONN_INFO *p_conn_data = NULL;

	if (COMMON_STATUS_OK != COMMON_HDL_getObject(*p_conn_handle, &p_conn_data))
	{
		p_conn_data = malloc(sizeof(CONN_INFO));
		result = COMMON_LIST_attachElement(g_CNM_cxt->conn_list, p_conn_data);

		COMMON_HDL_HandleCreate(p_conn_data, p_conn_handle);
		p_conn_data->conn_handle = *p_conn_handle;
	}

}
void thread_CNM_daemon(void)
{
	COMMON_HANDLE conn_handle;
	RETRIEVAL_TYPE retrieval_type;
	void *payload = NULL;
	CONN_INFO *p_conn_data = NULL;

	while (COMMON_STATUS_MSG_POP == CNM_process_listen(&conn_type))
	{
		CNM_monitor_read(conn_type, &payload_size, payload);
		/* Get transmission info from payload */
		conn_handle; /* Can be NULL for fresh request or anonymous request */
		user_name;
		password;
		retrieval_type;
		/* Get retrieval type specified JSON string */
		json_string;

		result = CNM_monitor_openConn(user_name, password, &conn_handle); /* Bear in mind the conn handle can be updated at this point. */
		COMMON_HDL_getObject(conn_handle, &p_conn_data);

		convertJSON2Struct(retrieval_type, json_string, &request_data);

		g_CNM_cxt.event_CB(conn_handle, retrieval_type, &request_data)
	}
}


SYSTEM_STATUS CNM_monitor_listen(CNM_CONN_TYPE *conn_type)
{
	SYSTEM_STATUS result = COMMON_STATUS_MSG_POP;
	struct pollfd *fds;
	uint32_t i;
	int posix_rv = -1;

	posix_rv = CNM_monitor_poll(g_CNM_cxt.fd_table, g_CNM_cxt.num_fds, -1);
	if (posix_rv > 0)
	{
		for (i = 0U; i < g_CNM_cxt.num_fds; i++)
		{
            if ((uint16_t)(g_CNM_cxt.fd_table[i].revents) & ((uint16_t)POLLHUP | (uint16_t)POLLERR | (uint16_t)POLLNVAL))
            {
                /* Wrong events! */
                COMMON_DIAG_WARN("Poll error, fd %i events 0x%x", g_CNM_cxt.fd_table[i].fd, g_CNM_cxt.fd_table[i].revents);
            }
            else if (((uint16_t)(g_CNM_cxt.fd_table[i].revents) & (uint16_t)POLLIN) ||
                     ((uint16_t)(g_CNM_cxt.fd_table[i].revents) & (uint16_t)POLLPRI))
            {
            	switch (i)
            	{
            	case CNM_CONN_TYPE_LOCAL:
            		*conn_type = CNM_CONN_TYPE_LOCAL;
            	break;

            	case CNM_CONN_TYPE_REMOTE:
            		*conn_type = CNM_CONN_TYPE_REMOTE;
            	break;
            	default:
            		*conn_type = CNM_CONN_TYPE_invalid;
            	break;
            	}
            }
            else
            {
                /* Unrecognised event */
            }

		}
	}

	return result;
}

CNM_monitor_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{}

CNM_monitor_read()
{}

/m()
{}
