/*
 * eLibrary_conn_manager.h
 * CNM = ConNection Manager
 *
 *  Created on: 1 Sep 2013
 *      Author: singlaive
 */

#ifndef ELIBRARY_CONN_MANAGER_H_
#define ELIBRARY_CONN_MANAGER_H_

typedef void (*CNM_event_CB)(COMMON_HANDLE conn_handle, RETRIEVAL_TYPE retrieval_type, void *pay_load);

typedef SYSTEM_STATUS (*addPlugin_func)(CNM_PLUGIN *plugin_interface);
typedef SYSTEM_STATUS (*rmPlugin_func)(CNM_PLUGIN *plugin_inteface);
typedef SYSTEM_STATUS (*registerEventCB_func)(CNM_event_listener listener);
typedef SYSTEM_STATUS (*sendData_func)(COMMON_HANDLE conn_handle, SERVER_RESPONSE response_data);

typedef struct
{
	addPlugin_func addPlugin;
	rmPlugin_func rmPlugin;
	registerEventCB_func registerEventCB;
	sendData_func sendData;
}CNM_INTERFACE;

extern CNM_INTERFACE g_CNM_interfaces;

SYSTEM_STATUS eLibrary_CNM_addlugIns();
SYSTEM_STATUS eLibrary_CNM_rmPlugIns();
SYSTEM_STATUS eLibrary_CNM_registerEventCB(CNM_event_CB listener);
SYSTEM_STATUS eLibrary_CNM_sendData(COMMON_HANDLE conn_handle, SERVER_RESPONSE response_data);

SYSTEM_STATUS eLibrary_CNM_SVR_init()
SYSTEM_STATUS eLibrary_CNM_SVR_term()

#endif /* ELIBRARY_CONN_MANAGER_H_ */
