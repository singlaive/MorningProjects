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

typedef USER_INFO * (*getUserInfo_func)(char * userName, char * email);
typedef USER_INFO * (*getFrdInfo_func)(USER_INFO * host, char * frdName);
typedef SYSTEM_STATUS (*getBookInfo_func)(BOOK_FILTER * filter, SORT_METHOD sort, uint32_t * numBooks, struct bookList ** books);
typedef SYSTEM_STATUS (*addUser_func)(char * userName, char * email);
typedef SYSTEM_STATUS (*addFriend_func)(USER_INFO * host, USER_INFO * friend);
typedef SYSTEM_STATUS (*rmFriend_func)(USER_INFO * host, char * frdName);
typedef SYSTEM_STATUS (*addBook_func)(USER_INFO * owner, char * bookName, char * bookDesp, BOOK_GROUP bookGroup);
typedef SYSTEM_STATUS (*rmBook_func)(USER_INFO * host, char * bookName);
typedef SYSTEM_STATUS (*addUserActy_func)(USER_INFO * host, ACTY_FILTER_TAG filter, char * name);
typedef SYSTEM_STATUS (*getUserActy_func)(USER_INFO * host, ACTY_FILTER_TAG filter, uint32_t * numActy, struct actyList ** activities);
typedef SYSTEM_STATUS (*getBookHty_func)(BOOK_INFO * book, ACTY_FILTER_TAG filter, uint32_t * numActy, struct actyList ** activities);
typedef SYSTEM_STATUS (*mergeActy_func)(USER_INFO * host, struct actyList * activities, uint32_t * counter);
typedef SYSTEM_STATUS (*wowBook_func)(USER_INFO * host, BOOK_INFO * book);
typedef SYSTEM_STATUS (*buyBook_func)(USER_INFO * host, BOOK_INFO * book);
typedef SYSTEM_STATUS (*getMsgInfo_func)(USER_INFO * host, char * frdName, struct msgList ** msgNode);
typedef SYSTEM_STATUS (*sendMsgInfo_func)(USER_INFO * host, char * frdName, char * msg);
typedef SYSTEM_STATUS (*notifyExchange_func)(USER_INFO * host, char * frdName, char * msg);
typedef SYSTEM_STATUS (*viewBook_func)(COMMON_HANDLE book_handle, char *buf);

typedef struct
{
	viewBook_func viewBook;
	addUser_func addUser;
	addFriend_func addFriend;
	rmFriend_func rmFriend;
	addBook_func addBook;
	rmBook_func rmBook;
	addUserActy_func addUserActy;
	getUserActy_func getUserActy;
	getBookHty_func getBookHty;
	mergeActy_func mergeActy;
	wowBook_func wowBook;
	buyBook_func buyBook;
	getMsgInfo_func getMsgInfo;
	sendMsgInfo_func sendMsgInfo;
	notifyExchange_func notifyExchange;
}HANDLER_METHODS;

#endif /* ELIBRARY_SERVERPROCESS_H_ */
