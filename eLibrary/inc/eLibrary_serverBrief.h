/*
 * eLibrary_serveerBrief.h
 *
 *  Created on: 1 Feb 2013
 *      Author: singlaive
 */

#ifndef ELIBRARY_SERVEERBRIEF_H_
#define ELIBRARY_SERVEERBRIEF_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <common.h>
#include "eLibrary_serverTypes.h"

char * briefEx(enum briefEVT briefEvent, void * ptr);
char * briefBooks(uint32_t numBooks, struct bookList * books);
char * briefActy(uint32_t numActy, struct actyList * activities);
char * briefGlabalBookHty(ACTY_FILTER_TAG acty_filter);
char * briefMsgBox(USER_INFO * host);
char * briefMsgContexts(struct msgList * msgNode);


#endif /* ELIBRARY_SERVEERBRIEF_H_ */
