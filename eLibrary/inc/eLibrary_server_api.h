/*
 * eLibrary_server.h
 *
 *  Created on: 18 Oct 2012
 *      Author: singlaive
 */

#ifndef ELIBRARY_SERVER_H_
#define ELIBRARY_SERVER_H_

#include "eLibrary_serverTypes.h"

SERVER_CONTEXT g_server_cxt;

SYSTEM_STATUS eLibrary_serverInit(CNM_INTERFACE cnm_interfaces, DB_ENGINE db_interfaces);
SYSTEM_STATUS eLibrary_serverTerm();

#endif /* ELIBRARY_SERVER_H_ */
