/*
 * test_eLibrary.h
 *
 *  Created on: 2013-2-5
 *      Author: singlaive
 */

#ifndef TEST_ELIBRARY_H_
#define TEST_ELIBRARY_H_

#include "eLibrary_serverTypes.h"

#define TEST_NUM_BOOKS 14U

struct testData
{
	char * bookName;//[MAX_BOOK_NAME_LENGTH];
	char * ownerName;//[MAX_NAME_LENGTH];
	char * email;//[MAX_EMAIL_LENGTH];
	char * desp;//[MAX_DESP_LENGTH];
	BOOK_GROUP group;
};

void test_serverHandler_userAdd(void);
void test_serverHandler_bookAdd(void);
void test_serverHandler_bookProp(void);
void test_serverHandler_createActy(void);
void test_serverHandler_msgs(void);
void test_serverHandler_checkActies(void);

#endif /* TEST_ELIBRARY_H_ */
