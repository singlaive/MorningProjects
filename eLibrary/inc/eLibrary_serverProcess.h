/*
 * eLibrary_serverProcess.h
 *
 *  Created on: 1 Feb 2013
 *      Author: singlaive
 */

#ifndef ELIBRARY_SERVERPROCESS_H_
#define ELIBRARY_SERVERPROCESS_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <common.h>
#include "eLibrary_serverTypes.h"

extern pthread_mutex_t lockUseInfo;
extern pthread_cond_t cond_exchange;

char * viewBook(char *);
BOOK_GROUP groupConverer(char *);
USER_INFO * getUserInfo(char * userName, char * email);
USER_INFO * getFrdInfo(USER_INFO * host, char * frdName);
SYSTEM_STATUS getBookInfo(BOOK_FILTER * filter, SORT_METHOD sort, uint32_t * numBooks, struct bookList ** books);
SYSTEM_STATUS addUser(char * userName, char * email);
SYSTEM_STATUS addFriend(USER_INFO * host, USER_INFO * friend);
SYSTEM_STATUS rmFriend(USER_INFO * host, char * frdName);
SYSTEM_STATUS addBook(USER_INFO * owner, char * bookName, char * bookDesp, BOOK_GROUP bookGroup);
SYSTEM_STATUS rmBook(USER_INFO * host, char * bookName);
SYSTEM_STATUS addUserActy(USER_INFO * host, ACTY_FILTER_TAG filter, char * name);
SYSTEM_STATUS getUserActy(USER_INFO * host, ACTY_FILTER_TAG filter, uint32_t * numActy, struct actyList ** activities);
SYSTEM_STATUS getBookHty(BOOK_INFO * book, ACTY_FILTER_TAG filter, uint32_t * numActy, struct actyList ** activities);
SYSTEM_STATUS mergeActy(USER_INFO * host, struct actyList * activities, uint32_t * counter);
SYSTEM_STATUS wowBook(USER_INFO * host, BOOK_INFO * book);
SYSTEM_STATUS buyBook(USER_INFO * host, BOOK_INFO * book);
SYSTEM_STATUS getMsgInfo(USER_INFO * host, char * frdName, struct msgList ** msgNode);
SYSTEM_STATUS sendMsgInfo(USER_INFO * host, char * frdName, char * msg);
SYSTEM_STATUS notifyExchange(USER_INFO * host, char * frdName, char * msg);

#endif /* ELIBRARY_SERVERPROCESS_H_ */
