/*
 * test_serverHandler.c
 *
 *  Created on: 10 Nov 2012
 *      Author: singlaive
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "common.h"
#include "eLibrary_serverHandler.h"
#include "eLibrary_serverProcess.h"
#include "CUnit/Automated.h"
#include "CUnit/Basic.h"
#include "test_eLibrary.h"
#include "eLibrary_server.h"

struct testData g_testData[TEST_NUM_BOOKS] =
{
	{"Alex's Book 01", "Alex", "alex@gmail.com", "This is Alex's first book.", FICTION},
	{"Alex's Book 02", "Alex", "alex@gmail.com", NULL, LITERATURE},
	{"Alex's Book 03", "Alex", "alex@gmail.com", NULL, book_group_undef},
	{"Bob's Book 01", "Bob", "bob@gmail.com", NULL, LITERATURE},
	{"Bob's Book 02", "Bob", "bob@gmail.com", "This is Bob's second book.", FICTION},
	{"Bob's Book 03", "Bob", "bob@gmail.com", "This is Bob's third book.", FICTION},
	{"Clair's Book 01", "Clair", "clair@gmail.com", "This is Clair's first book.", FICTION},
	{"Clair's Book 02", "Clair", "clair@gmail.com", "This is Clair's second book.", LITERATURE},
	{"Clair's Book 03", "Clair", "clair@gmail.com", "This is Clair's third book.", LITERATURE},
	{"Clair's Book 04", "Clair", "clair@gmail.com", "This is Clair's fourth book.", LITERATURE},
	{"Dan's Book 01", "Dan", "dan@gmail.com", NULL, LITERATURE},
	/* Here below are bad recordes. */
	{"Dan's Book 04", "Dan", NULL, NULL, LITERATURE},
	{NULL, NULL, NULL, NULL, LITERATURE},
	{NULL, "Dan", NULL, NULL, LITERATURE},
};

void test_dumpFrdInfo(USER_INFO * host)
{
	struct userList * current = host->friends;
	uint32_t i = 0U;

	while (NULL != current)
	{
		DIAG_INFO("Friend #%d: %s", ++i, host->friends->user->username);
		current = current->next;
	}

	if (0 == i)
		DIAG_INFO("%s does not have any friends! Sad for you ~~~", host->username);
}

SYSTEM_STATUS test_wowBuyBook(uint32_t i, uint32_t j, ACTY_FILTER_TAG acty)
{
	SYSTEM_STATUS result = STATUS_FAIL;
	USER_INFO * userInfo = NULL;
	BOOK_FILTER filter;
	uint32_t numBooks = 0U;
	struct bookList * bookList = NULL;

	userInfo = (USER_INFO *)getUserInfo(g_testData[i].ownerName, NULL); /* Don'd know why we need this casting, otherwise there will be a compile warning. */
	CU_ASSERT_NOT_EQUAL(userInfo, NULL);

	filter.tag = FILTER_BY_BOOK_NAME;
	filter.filter.bookName = g_testData[j].bookName;
	result = getBookInfo(&filter, sort_method_undef, &numBooks, &bookList);
	CU_ASSERT_EQUAL(result, STATUS_OK);

	if (0U == numBooks) result = STATUS_FAIL;

	if ((NULL != userInfo) && (result == STATUS_OK))
	{
		if (ACTY_FILTER_BY_WOW == acty)
		{
			result = wowBook(userInfo, bookList->book);
		}
		else
		{
			result = buyBook(userInfo, bookList->book);
		}

		if (STATUS_OK != result)
		{
			DIAG_ERROR("handleWow/Buy() failed.");
		}
	}

	COMMON_free(bookList);

	return result;
}

void test_serverHandler_userAdd(void)
{
	uint32_t i;
	SYSTEM_STATUS result = STATUS_FAIL;
	USER_INFO * host = NULL, * friend = NULL;

	printf(">>>>>>>>>>>>>>>>>\n");
	DIAG_INFO("TEST: Add users.");

	for (i = 0U; i < (TEST_NUM_BOOKS-3U); i++)
	{

		if (NULL == getUserInfo(g_testData[i].ownerName, NULL))
		{
			DIAG_INFO("To addUser() (name = %s; email = %s)", g_testData[i].ownerName, g_testData[i].email);

			result = addUser(g_testData[i].ownerName, g_testData[i].email);
			if (STATUS_OK != result)
			{
				DIAG_ERROR("Failed addUser() (name = %s; email = %s)", g_testData[i].ownerName, g_testData[i].email);
				CU_FAIL("Failed addUser()");
			}
		}
	}

	/* Negative test. */
	for (i = (TEST_NUM_BOOKS-3U); i < TEST_NUM_BOOKS; i++)
	{
		DIAG_INFO("To addUser() (name = %s; email = %s)", g_testData[i].ownerName, g_testData[i].email);
		result = addUser(g_testData[i].ownerName, g_testData[i].email); /* i = 11, 12, 13*/
		CU_ASSERT_NOT_EQUAL(STATUS_OK, result);
	}

	DIAG_INFO("TEST: Add friends.");

	/* Alex's friends: Bob, Clair and Dan;
	 * Bob's friends: Clair and Dan;
	 * Chris's friends: Dan;
	 * Dan don't have any friends.
	 */
	host = getUserInfo(g_testData[0U].ownerName, NULL);
	friend = getUserInfo(g_testData[3U].ownerName, NULL);

	if ((NULL != host) && (NULL != friend))
	{
		result = addFriend(host, friend);
		if (STATUS_OK != result)
		{
			DIAG_ERROR("Failed addFriend() (host = %s; friend = %s)", g_testData[0U].ownerName, g_testData[3U].ownerName);
			CU_FAIL("Failed addUser()");
		}
	}

	if (NULL != (friend = getUserInfo(g_testData[6U].ownerName, NULL)))
	{
		result = addFriend(host, friend);
		if (STATUS_OK != result)
		{
			DIAG_ERROR("Failed addFriend() (host = %s; friend = %s)", g_testData[0U].ownerName, g_testData[3U].ownerName);
			CU_FAIL("Failed addUser()");
		}
	}

	if (NULL != (friend = getUserInfo(g_testData[10U].ownerName, NULL)))
	{
		result = addFriend(host, friend);
		if (STATUS_OK != result)
		{
			DIAG_ERROR("Failed addFriend() (host = %s; friend = %s)", g_testData[0U].ownerName, g_testData[3U].ownerName);
			CU_FAIL("Failed addUser()");
		}
	}

	CU_ASSERT_EQUAL(host->numFrds, 3U);
	test_dumpFrdInfo(host);

	if ((NULL != (host = getUserInfo(g_testData[3U].ownerName, NULL))) && (NULL != (friend = getUserInfo(g_testData[6U].ownerName, NULL))))
	{
		result = addFriend(host, friend);
		if (STATUS_OK != result)
		{
			DIAG_ERROR("Failed addFriend() (host = %s; friend = %s)", g_testData[0U].ownerName, g_testData[3U].ownerName);
			CU_FAIL("Failed addUser()");
		}
	}

	if (NULL != (friend = getUserInfo(g_testData[10U].ownerName, NULL)))
	{
		result = addFriend(host, friend);
		if (STATUS_OK != result)
		{
			DIAG_ERROR("Failed addFriend() (host = %s; friend = %s)", g_testData[0U].ownerName, g_testData[3U].ownerName);
			CU_FAIL("Failed addUser()");
		}
	}

	CU_ASSERT_EQUAL(host->numFrds, 2U);
	test_dumpFrdInfo(host);

	if ((NULL != (host = getUserInfo(g_testData[6U].ownerName, NULL))) && (NULL != (friend = getUserInfo(g_testData[10U].ownerName, NULL))))
	{
		result = addFriend(host, friend);
		if (STATUS_OK != result)
		{
			DIAG_ERROR("Failed addFriend() (host = %s; friend = %s)", g_testData[0U].ownerName, g_testData[3U].ownerName);
			CU_FAIL("Failed addUser()");
		}
	}

	CU_ASSERT_EQUAL(host->numFrds, 1U);
	test_dumpFrdInfo(host);
	CU_ASSERT_EQUAL(friend->numFrds, 0U);
	test_dumpFrdInfo(friend);


	/* Don't know how to test getFrdInfo(). */

	printf("<<<<<<<<<<<<<<<<<\n");
}

void test_serverHandler_bookAdd(void)
{
	uint32_t i;
	SYSTEM_STATUS result = STATUS_FAIL;
	USER_INFO * user = NULL;

	DIAG_INFO("TEST: Add books.");
	printf(">>>>>>>>>>>>>>>>>\n");

	for (i = 0U; i < (TEST_NUM_BOOKS-3U); i++)
	{
		user = getUserInfo(g_testData[i].ownerName, NULL);
		CU_ASSERT_NOT_EQUAL(user, NULL);

		result = addBook(user, g_testData[i].bookName, g_testData[i].desp, g_testData[i].group);
		if (STATUS_OK != result)
		{
			DIAG_ERROR("Failed addBook() (i = %d)", i);
			CU_FAIL("Failed addBook()");
		}
	}

	for (i = (TEST_NUM_BOOKS-3U); i < TEST_NUM_BOOKS; i++)
	{
		result = addBook(NULL, g_testData[i].bookName, g_testData[i].desp, g_testData[i].group);
		CU_ASSERT_NOT_EQUAL(STATUS_OK, result);
	}

	printf("<<<<<<<<<<<<<<<<<\n");
}

void test_serverHandler_bookProp(void)
{
	SYSTEM_STATUS result = STATUS_FAIL;
	BOOK_FILTER filter;
	uint32_t numBooks = 0U;
	USER_INFO * userInfo = NULL;
	struct bookList * bookList = NULL;

	printf(">>>>>>>>>>>>>>>>>\n");
	DIAG_INFO("TEST: Book property retrieval.");

	/* TEST: Filter by book name. */
	filter.tag = FILTER_BY_BOOK_NAME;
	filter.filter.bookName = g_testData[0U].bookName;
	result = getBookInfo(&filter, sort_method_undef, &numBooks, &bookList);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(numBooks, 1);
	if (STATUS_OK == result)
		briefBooks(numBooks, bookList);

	COMMON_free(bookList);
	bookList = NULL;

#if 0 /*Not test until fnishing sorting. */
	/* TEST: Filter by user name. */
	userInfo = getUserInfo(g_testData[0U].ownerName, NULL);
	CU_ASSERT_NOT_EQUAL(userInfo, NULL);
	if (NULL != userInfo)
	{
		filter.tag = FILTER_BY_USER;
		filter.filter.user = userInfo;
		result = getBookInfo(&filter, SORT_BY_DATE, &numBooks, &bookList);
		CU_ASSERT_EQUAL(result, STATUS_OK);
		CU_ASSERT_EQUAL(numBooks, 3U);
		if (STATUS_OK != result)
			briefBooks(numBooks, bookList);
		COMMON_free(bookList);
		bookList = NULL;

		result = getBookInfo(&filter, SORT_BY_ALFA, &numBooks, &bookList);
		CU_ASSERT_EQUAL(result, STATUS_OK);
		if (STATUS_OK != result)
			briefBooks(numBooks, bookList);
		COMMON_free(bookList);
		bookList = NULL;

		result = getBookInfo(&filter, SORT_BY_GROUP, &numBooks, &bookList);
		CU_ASSERT_EQUAL(result, STATUS_OK);
		if (STATUS_OK != result)
			briefBooks(numBooks, bookList);
		COMMON_free(bookList);
		bookList = NULL;printf("<<<<<<<<<<<<<<<<<\n");
	}
#endif

}

void test_serverHandler_createActy(void)
{
	SYSTEM_STATUS result = STATUS_FAIL;

	printf(">>>>>>>>>>>>>>>>>\n");
	DIAG_INFO("TEST: Create activities.");

	/* One can not WOW its own book. */
	result = test_wowBuyBook(1, 2, ACTY_FILTER_BY_WOW);
	CU_ASSERT_EQUAL(result, STATUS_FAIL);

	result = test_wowBuyBook(1, 3, ACTY_FILTER_BY_WOW);
	CU_ASSERT_EQUAL(result, STATUS_OK);

	result = test_wowBuyBook(1, 4, ACTY_FILTER_BY_WOW);
	CU_ASSERT_EQUAL(result, STATUS_OK);

	result = test_wowBuyBook(4, 2, ACTY_FILTER_BY_WOW);
	CU_ASSERT_EQUAL(result, STATUS_OK);

	result = test_wowBuyBook(4, 7, ACTY_FILTER_BY_WOW);
	CU_ASSERT_EQUAL(result, STATUS_OK);

	result = test_wowBuyBook(6, 2, ACTY_FILTER_BY_WOW);
	CU_ASSERT_EQUAL(result, STATUS_OK);

	result = test_wowBuyBook(6, 4, ACTY_FILTER_BY_WOW);
	CU_ASSERT_EQUAL(result, STATUS_OK);

	result = test_wowBuyBook(1, 4, ACTY_FILTER_BY_BUY);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	result = test_wowBuyBook(4, 2, ACTY_FILTER_BY_BUY);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	result = test_wowBuyBook(6, 1, ACTY_FILTER_BY_BUY);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	result = test_wowBuyBook(6, 2, ACTY_FILTER_BY_BUY);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	result = test_wowBuyBook(6, 4, ACTY_FILTER_BY_BUY);
	CU_ASSERT_EQUAL(result, STATUS_OK);

	/* Negative tests */
	result = test_wowBuyBook(1, 1, ACTY_FILTER_BY_BUY);
	CU_ASSERT_EQUAL(result, STATUS_FAIL);

	result = test_wowBuyBook(1, 1, ACTY_FILTER_BY_WOW);
	CU_ASSERT_EQUAL(result, STATUS_FAIL);

	result = test_wowBuyBook(1, 11, ACTY_FILTER_BY_WOW);
	CU_ASSERT_EQUAL(result, STATUS_FAIL);

	printf("<<<<<<<<<<<<<<<<<\n");
}

void test_serverHandler_msgs(void)
{
	USER_INFO * userInfo = NULL;
	uint32_t numMsg = 0U;
	uint32_t numUnread = 0U;
	struct msgContext * contexts = NULL;
	SYSTEM_STATUS result = STATUS_FAIL;
	struct msgList * msgNode = NULL;

	sleep(1);
	printf(">>>>>>>>>>>>>>>>>\n");
	DIAG_INFO("TEST: Send/Receive messages.");
	/* At the beginning, no msgs at all. */
	userInfo = getUserInfo(g_testData[0].ownerName, NULL);
	result = getMsgInfo(userInfo, NULL, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_INVALID_PARAM);

	result = getMsgInfo(userInfo, userInfo->friends->user->username, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(msgNode, NULL);

	/* Alex said hello to his first friend Bob. */
	result = sendReceiveMsg(userInfo, userInfo->friends->user->username, "Mother fucker!", MSG_RECEIPT_OUT);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	sleep(1);
	result = getMsgInfo(userInfo, userInfo->friends->user->username, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(msgNode->numMsg, 1U);
	CU_ASSERT_EQUAL(msgNode->numUnread, 0U);
	CU_ASSERT_NOT_EQUAL(msgNode->contexts, NULL);
	briefMsgContexts(msgNode);

	/* Bob shall has a unread message now. */
	result = getMsgInfo(userInfo->friends->user, userInfo->username, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(msgNode->numMsg, 1U);
	CU_ASSERT_EQUAL(msgNode->numUnread, 1U);
	CU_ASSERT_NOT_EQUAL(msgNode->contexts, NULL);
	briefMsgContexts(msgNode);
	/* After Bob read it, there is no unread message. */
	result = getMsgInfo(userInfo->friends->user, userInfo->username, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(msgNode->numMsg, 1U);
	CU_ASSERT_EQUAL(msgNode->numUnread, 0U);
	CU_ASSERT_NOT_EQUAL(msgNode->contexts, NULL);

	result = sendReceiveMsg(userInfo->friends->user, userInfo->username, "Hi swetty!", MSG_RECEIPT_OUT);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	sleep(1);
	result = getMsgInfo(userInfo, userInfo->friends->user->username, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(msgNode->numMsg, 2U);
	CU_ASSERT_EQUAL(msgNode->numUnread, 1U);
	CU_ASSERT_NOT_EQUAL(msgNode->contexts, NULL);
	briefMsgContexts(msgNode);
	CU_ASSERT_EQUAL(msgNode->numUnread, 0U);

	result = sendReceiveMsg(userInfo, userInfo->friends->user->username, "Get lay down?", MSG_RECEIPT_OUT);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	sleep(1);
	result =getMsgInfo(userInfo, userInfo->friends->user->username, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(msgNode->numMsg, 3U);
	CU_ASSERT_EQUAL(msgNode->numUnread, 0U);
	CU_ASSERT_NOT_EQUAL(msgNode->contexts, NULL);
	briefMsgContexts(msgNode);

	result = sendReceiveMsg(userInfo->friends->user, userInfo->username, "May I have your phone no!", MSG_RECEIPT_OUT);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	sleep(1);
	result =getMsgInfo(userInfo, userInfo->friends->user->username, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(msgNode->numMsg, 4U);
	CU_ASSERT_EQUAL(msgNode->numUnread, 1U);
	CU_ASSERT_NOT_EQUAL(msgNode->contexts, NULL);
	briefMsgContexts(msgNode);
	CU_ASSERT_EQUAL(msgNode->numUnread, 0U);

	result = sendReceiveMsg(userInfo->friends->next->user, userInfo->username, "How's going!", MSG_RECEIPT_OUT);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	sleep(1);
	result = getMsgInfo(userInfo, userInfo->friends->next->user->username, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(msgNode->numMsg, 1U);
	CU_ASSERT_EQUAL(msgNode->numUnread, 1U);
	CU_ASSERT_NOT_EQUAL(msgNode->contexts, NULL);
	briefMsgContexts(msgNode);
	CU_ASSERT_EQUAL(msgNode->numUnread, 0U);

	result = getMsgInfo(userInfo->friends->next->user, userInfo->username, &msgNode);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_EQUAL(msgNode->numMsg, 1U);
	CU_ASSERT_EQUAL(msgNode->numUnread, 0U);
	CU_ASSERT_NOT_EQUAL(msgNode->contexts, NULL);
	briefMsgContexts(msgNode);

	DIAG_INFO("Name: %s, msg %d, unread %d", userInfo->username, userInfo->msgBox.numMsg, userInfo->msgBox.numUnread);
	CU_ASSERT_EQUAL(userInfo->msgBox.numMsg, 5U);
	CU_ASSERT_EQUAL(userInfo->msgBox.numUnread, 3U); /* Unread number ins msg box is updated in HandleMsg() not lower API. */
	DIAG_INFO("Name: %s, msg %d, unread %d", userInfo->friends->user->username, userInfo->friends->user->msgBox.numMsg, userInfo->friends->user->msgBox.numUnread);
	CU_ASSERT_EQUAL(userInfo->friends->user->msgBox.numMsg, 4U);
	CU_ASSERT_EQUAL(userInfo->friends->user->msgBox.numUnread, 2U); /* Unread number ins msg box is updated in HandleMsg() not lower API. */
	DIAG_INFO("Name: %s, msg %d, unread %d", userInfo->friends->next->user->username, userInfo->friends->next->user->msgBox.numMsg, userInfo->friends->next->user->msgBox.numUnread);
	CU_ASSERT_EQUAL(userInfo->friends->next->user->msgBox.numMsg, 1U);
	CU_ASSERT_EQUAL(userInfo->friends->next->user->msgBox.numUnread, 0U); /* Unread number ins msg box is updated in HandleMsg() not lower API. */

	briefMsgBox(userInfo);
	briefMsgBox(userInfo->friends->user);
	briefMsgBox(userInfo->friends->next->user);
	printf("<<<<<<<<<<<<<<<<<\n");
}

void test_serverHandler_checkActies(void)
{
	SYSTEM_STATUS result = STATUS_FAIL;
	USER_INFO * userInfo = NULL;
	ACTY_FILTER_TAG filter = acty_filter_undef;
	BOOK_FILTER book_filter;
	uint32_t counter = 0U;
	struct actyList * activities = NULL;
	struct bookList * books = NULL;
	uint32_t numBooks = 0U;

	printf(">>>>>>>>>>>>>>>>>\n");
	DIAG_INFO("TEST: Check activities.");

	userInfo = getUserInfo(g_testData[0].ownerName, NULL);

	filter = ACTY_FILTER_ALL;
	result = getUserActy(userInfo, filter, &counter, &activities);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_NOT_EQUAL(counter, 0U);
	CU_ASSERT_NOT_EQUAL(activities, NULL);
	briefActy(counter, activities);

	filter = ACTY_FILTER_BY_WOW;
	result = getUserActy(userInfo, filter, &counter, &activities);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_NOT_EQUAL(counter, 0U);
printf("*******************************************************************counter = %d\n", counter);
	CU_ASSERT_NOT_EQUAL(activities, NULL);
	briefActy(counter, activities);

	filter = ACTY_FILTER_BY_BUY;
	result = getUserActy(userInfo, filter, &counter, &activities);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_NOT_EQUAL(counter, 0U);
	printf("*******************************************************************counter = %d\n", counter);
	CU_ASSERT_NOT_EQUAL(activities, NULL);
	briefActy(counter, activities);

	printf("book: %s\n", g_testData[0].bookName);
	book_filter.tag = FILTER_BY_BOOK_NAME;
	book_filter.filter.bookName = g_testData[0].bookName;
	result = getBookInfo(&book_filter, sort_method_undef, &numBooks, &books);
	CU_ASSERT_NOT_EQUAL(numBooks, 0U);


	filter = ACTY_FILTER_BY_BUY;
	result = getBookHty(books->book, filter, &counter, &activities);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_NOT_EQUAL(counter, 0U);
	CU_ASSERT_NOT_EQUAL(activities, NULL);
	briefActy(counter, activities);

	filter = ACTY_FILTER_BY_WOW;
	result = getBookHty(books->book, filter, &counter, &activities);
	CU_ASSERT_EQUAL(result, STATUS_OK);
	CU_ASSERT_NOT_EQUAL(counter, 0U);
	CU_ASSERT_NOT_EQUAL(activities, NULL);
	briefActy(counter, activities);
}






















