/*
 * eLibrary_serverProcess.h
 *
 *  Created on: 18 Oct 2012
 *      Author: singlaive
 */

#ifndef ELIBRARY_SERVERHANDLER_H_
#define ELIBRARY_SERVERHANDLER_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"
#include "eLibrary_serverTypes.h"



char * eLibrary_cmdProcess(char * userName, char * cmdStr);
void * thread_processMsg(void * p);


SYSTEM_STATUS server_msgSend();
void actyFree(ACTY_FILTER_TAG filter, struct actyList * activities, uint32_t counter);


char * eLibrary_server_ParserView();
char * eLibrary_server_ParserAdd();
char * eLibrary_server_ParserRm();
char * eLibrary_server_ParserSearch();
char * eLibrary_server_ParserWhatsup();
char * eLibrary_server_ParserWow();
char * eLibrary_server_ParserBuy();
char * eLibrary_server_ParserMsg();

HANDLER_METHODS g_eLibrary_server_handler;



#endif /* ELIBRARY_SERVERHANDLER_H_ */
