/*
 * eLibrary_server_process.h
 *
 *  Created on: 6 Feb 2014
 *      Author: murphy
 */


/*
 * eLibrary_server_process.c
 *
 *  Created on: 5 Feb 2014
 *      Author: murphy
 */
#include "common_types.h"
#include "common_handle.h"
#include "eLibrary_sharedTypes.h"

void thread_SVR_daemon(void);
void thread_SVR_worker(void);
void CNM_event_listener(COMMON_HANDLE conn_handle, RETRIEVAL_TYPE retrieval_type, void *pay_load);
