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
char * handlerView(char * userName, cmd_line_tag * parsedCmd);
char * handlerAdd(char * userName, cmd_line_tag * parsedCmd);
char * handlerRm(char * userName, cmd_line_tag * parsedCmd);
char * handlerSearch(char * userName, cmd_line_tag * parsedCmd);
char * handlerWhatsup(char * userName, cmd_line_tag * parsedCmd);
char * handlerWow(char * userName, cmd_line_tag * parsedCmd);
char * handlerBuy(char * userName, cmd_line_tag * parsedCmd);
char * handlerMsg(char * userName, cmd_line_tag * parsedCmd);

SYSTEM_STATUS server_msgSend();
void actyFree(ACTY_FILTER_TAG filter, struct actyList * activities, uint32_t counter);






#endif /* ELIBRARY_SERVERHANDLER_H_ */
