/*
 * eLibrary_CNM_client_API.c
 *
 *  Created on: 1 Sep 2013
 *      Author: singlaive
 */

#include "common_types.h"
#include "eLibrary_sharedTypes.h"
#include "eLibrary_CNM_client_API.h"

CNM_CLT_CONTEXT g_CNM_client_cxt;

SYSTEM_STATUS eLibrary_CNM_CLT_setProtocol_func(CNM_CLT_CONN_PROTOCOL conn_protocol);
SYSTEM_STATUS eLibrary_CNM_CLT_registerEventCB(COMMON_HADLE client_id, CNM_CLT_event_CB listener);
SYSTEM_STATUS eLibrary_CNM_CLT_sendData(CLIENT_REQUEST *p_request);

CNM_CLT_INTERFACE g_CNM_client_interfaces =
{
    eLibrary_CNM_CLT_setProtocol,
	eLibrary_CNM_CLT_registerEventCB,
	eLibrary_CNM_CLT_sendData
};

SYSTEM_STATUS eLibrary_CNM_CLT_init()
{
    SYSTEM_STATUS result;

	result = COMMON_threadCreate(thread_CNM_CLT_monitor);
}

SYSTEM_STATUS eLibrary_CNM_term()
{
}

SYSTEM_STATUS eLibrary_CNM_CLT_setIdentity(char *user_name, char *password)
{}

SYSTEM_STATUS eLibrary_CNM_CLT_setProtocol(CNM_CLT_PROTOCOL conn_protocol)
{

	switch (conn_protocol)
	{
		case CNM_CLT_PROTOCOL_FIFO:
            char fifo_file[MAX_FILE_NAME_LENGTH];

            g_CNM_client_cxt.protocol = conn_protocol;
            sprintf(fifo_file, "/tmp/eLibrary_fifo%d", 0);

			while (!(fd = open(fifo_file, O_RDONLY)))
			{
                close(fd);
                            
                sprintf(fifo_file, "/tmp/eLibrary_fifo%d", i+1U);
			}
            
            g_CNM_client_cxt->conn_meta.conn.fifo_fd = open(fifo_file, O_READ|O_WRITE|O_CREATE);
        break;

		case CNM_PROTOCOL_UDP:
		case CNM_PROTOCOL_TCP:
		case CNM_PROTOCOL_HTTP:
		case CNM_PROTOCOL_UPNP:
		default:
			COMMON_DIAG_ERROR("Unsupported prototol %d.", conn_protocol);
	}
}
SYSTEM_STATUS eLibrary_CNM_CLT_register_client(COMMON_HANDLE client_id)
{}

SYSTEM_STATUS eLibrary_CNM_CLT_registerEventCB(COMMON_HADLE client_id, CNM_CLT_event_CB listener);
{
	g_CNM_client_cxt.event_CB = listener;
}

SYSTEM_STATUS eLibrary_CNM_CLT_createRequest(CLIENT_REQUEST *p_request, COMMON_HANDLE *request_handle);
{
}

SYSTEM_STATUS eLibrary_CNM_CLT_sendRequest(COMMON_HANDLE request_handle)
{}

SYSTEM_STATUS eLibrary_CNM_CLT_destroyRequest(COMMON_HANDLE request_handle)
{}



