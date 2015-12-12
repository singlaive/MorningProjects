/*
 * eLibrary_conn_manager.c
 *
 *  Created on: 1 Sep 2013
 *      Author: singlaive
 */

#include "common_types.h"
#include "eLibrary_CNM_API.h"


CNM_CONTEXT g_CNM_cxt;

CNM_INTERFACE g_CNM_interfaces =
{
		eLibrary_CNM_addPlugIn,
		eLibrary_CNM_rmlugIn,
		eLibrary_CNM_registerEventCB,
		eLibrary_CNM_sendData
};

SYSTEM_STATUS eLibrary_CNM_init()
{
	/* setup shared memory, or fifo */
	/* setup socket */
	g_CNM_cxt.fd_table[0] = g_CNM_cxt.fifo_fd;
	g_CNM_cxt.fd_table[1] = g_CNM_cxt.socket_fd;

	for (i = 0U; i < g_CNM_cxt.num_fds; i++)
	{
		g_CNM_cxt.fd_table[i].events = POLLIN;
		g_CNM_cxt.fd_table[i].revents = 0;
	}

	result = COMMON_threadCreate(thread_CNM_daemon, NULL);
}

SYSTEM_STATUS eLibrary_CNM_term()
{

}
SYSTEM_STATUS eLibrary_CNM_addlugIn();
SYSTEM_STATUS eLibrary_CNM_rmPlugIn();

SYSTEM_STATUS eLibrary_CNM_registerEventCB(CNM_event_CB listener)
{
	g_CNM_cxt.event_CB = listener;
}
SYSTEM_STATUS eLibrary_CNM_sendData(COMMON_HANDLE conn_handle, SERVER_RESPONSE response_data)
{
	CNM_monitor_write();
}
