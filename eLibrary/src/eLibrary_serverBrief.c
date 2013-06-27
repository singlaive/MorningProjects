/*
 * eLibrary_serverBrief.c
 *
 *  Created on: 19 Dec 2012
 *      Author: singlaive
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
//#include "eLibrary_serverHandler.h"
#include "eLibrary_serverBrief.h"

#define TEST_BRIEF

char * briefEx(enum briefEVT briefEvent, void * ptr)
{
	char * rtnMsg = NULL;

	DIAG_INFO("briefEvent = %d; ptr = %#x", briefEvent, (uint32_t)ptr);

	rtnMsg = (char *)malloc(MAX_BRIEF_LENGTH*sizeof(char));
	if (NULL == rtnMsg)
	{
		DIAG_ERROR("Failed to allocate memory.");
		return rtnMsg;
	}

	if ((NULL == ptr) || (briefEvent < brief_evt_undef) || (briefEvent > brief_evt_lastvalue))
	{
		DIAG_ERROR("Invalid parameter");
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "System error.\n");
		return rtnMsg;
	}

	switch (briefEvent)
	{
	case BRIEF_EVT_HLP_VIEW:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Usage: view [option]... [target]\n\
								List info of a user or books. Without any options or parameters, it gives a brief info about the current user.\n\n\
								-b 		list a book by name. Without any parameter, it gives all books the current user collect.\n\
								-a		list books in alphabetical order. By default it is by date. \n\
				                -g		list books in group.\n\
				                -f		list a brief info for a friend of the current user. Without any parameter it shows all friends.\n\
				                @eLibrary 2012");
		break;

	case BRIEF_EVT_HLP_ADD:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Usage: add option [content] target\n\
				                Add a book or some specific info to a book or, add a friend.\n\n\
				                -b		add a book by name.\n\
				                -d		add a description to a book.\n\
				                -g		group a book.\n\
				                -f		add a friend.\n\
				                @eLibrary 2012");
		break;

	case BRIEF_EVT_HLP_RM:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Usage: remove option target\n\
				                Remove a book from collection, or delete a friend. Note when removing a book, it does not actually delete the book.\n\n\
				                -b		remove a book by name.\n\
				                -f		remove a friend.\n\
				                @eLibrary 2012");
		break;

	case BRIEF_EVT_HLP_SEARCH:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Usage: search option target\n\
				                Search a book, or search a user.\n\n\
				                -b		search a book.\n\
				                -f		search a user\n\
				                @eLibrary 2012");
		break;

	case BRIEF_EVT_HLP_WHATSUP:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Usage: whatsup [option] [target]\n\
				                List recent activities. Without any options or parameters it shows activities of all the current user's and his friends'.\n\n\
				                -f		list recent activities of a friend.\n\
								@eLibrary 2012");
		break;

	case BRIEF_EVT_HLP_WOW:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Usage: wow [option] [target]\n\
				                Add a WOW tag to a book or view who WOWed this book. \n\
				                Without any options or parameter it shows books WOWed recently.\n\
				                \"wow target\", shall add a WOW tag to this book. \n\
				                -f		view the recently WOWed books by a friend.\n\
				                -b		view the WOWed history of a book\n\
				                @eLibrary 2012");
		break;

	case BRIEF_EVT_HLP_BUY:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Usage: buy [option] [target]\n\
				                Add a BUY tag to a book or view who BUYed this book. \n\
				                Without any options or parameter it shows books BUYed recently.\n\
				                \"buy target\", shall add a BUY tag to this book. \n\
				                -f		view the recently BUYed books by a friend.\n\
				                -b		view the BUYed history of a book\n\
				                @eLibrary 2012");
		break;

	case BRIEF_EVT_HLP_MSG:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Usage: msg [option] [target]\n\
				                View the msg inbox or send msg to a friend. Without any options or parameters it show the inbox of current user.\n\n\
				                -s send a piece of message to a friend.\n\
				                -f view all messages with this friend.\n\
				                @eLibrary 2012");
	    break;

	case BRIEF_EVT_VIEW_USER:
	{
		USER_INFO * user = ptr;
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "User: %s; email: %s\nThere are %d books on his book shelf.\nHe has %d friends.\nThere are %d msg unread in the Inbox.", user->username, user->email, user->numBooks, user->numFrds, user->msgBox.numUnread);
	}
		break;

	case BRIEF_EVT_VIEW_BOOK:
	{
		BOOK_INFO * book = ptr;
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Book: %s\nGroup: %d\n %d persons collected this book\n%d persons WOWed this book\n%d persons BUYed this book\nDescription: %s", book->name, book->group, book->countCollected, book->numWow, book->numBuy, book->desp);
	}
		break;

	case BRIEF_EVT_VIEW_FRD_ALL:
	{
		USER_INFO * user = ptr;
		uint32_t count = 0U;
		struct userList * current = user->friends;
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "%s has %d friends:\n", user->username, user->numFrds);
		while (NULL == current)
		{
			char * buff;
			snprintf(buff, MAX_BRIEF_LENGTH, "	%s %d books on shelf %d friends.\n", current->user->username, current->user->numBooks, current->user->numFrds);
			strncat(rtnMsg, buff, MAX_BRIEF_LENGTH);
			current = current->next;
			count++;
		}

		if (count != user->numFrds)
		{
			DIAG_WARN("Count of friends is not correct (%d, %d).", count, user->numFrds);
		}
	}
		break;

	case BRIEF_EVT_ADD_FRIEND:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "%s is now your friend.", (char *)ptr);
		break;

	case BRIEF_EVT_ADD_DESP:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Description added to book %s.", (char *)ptr);
		break;

	case BRIEF_EVT_ADD_GROUP:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Group set for book %s.", (char *)ptr);
		break;

	case BRIEF_EVT_ADD_BOOK:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Book %s is now on your shelf.", (char *)ptr);
		break;

	case BRIEF_EVT_RM_FRIEND:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "%s removed from your friends.", (char *)ptr);
		break;

	case BRIEF_EVT_RM_BOOK:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "%s removed from your shelf.", (char *)ptr);
		break;

	case BRIEF_EVT_WOW_BOOK:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "WOW tag added to book %s.", (char *)ptr);
		break;

	case BRIEF_EVT_BUY_BOOK:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "BUY tag added to book %s.", (char *)ptr);
		break;

	case BRIEF_EVT_ERR_CMD_INVALID:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "Invalid command. Add -h for usetage.\n");
		break;

	case BRIEF_EVT_ERR_GENERAL:
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "System error.\n");
		break;

	default:
		DIAG_WARN("Invalid brirf event (%d).", briefEvent);
		break;
	}

#ifdef TEST_BRIEF
	DIAG_INFO("Msg (event: %d) to be briefed is:", briefEvent);
	printf("%s", rtnMsg);
#endif

	return rtnMsg;
}

/* TBD: MAX_BRIEF_LENGTH might still not be big enough. Take care of overflow of rtnmsg. */

char * briefBooks(uint32_t numBooks, struct bookList * books)
{
	uint32_t count = 0U;
	struct bookList * current = books;
	char * rtnMsg = NULL;

	DIAG_INFO("numBooks = %d; books = %#x", numBooks, (uint32_t)books);

	rtnMsg = (char *)malloc(MAX_BRIEF_LENGTH*sizeof(char));
	if (NULL == rtnMsg)
	{
		DIAG_ERROR("Failed to allocate memory.");
	}
	else if ((0U == numBooks) || (NULL == books))
	{
		DIAG_INFO("No books.");
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "There are no books on the shelf yet!\n");
	}
	else
	{
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "There are %d books on the shelf.", numBooks);
		while (NULL != current)
		{
			char buff[MAX_ACTY_LENGTH];
			snprintf(buff, MAX_ACTY_LENGTH, "%s [%d] %d WOWs %d BUYs.\n", current->book->name, current->book->group, current->book->numWow, current->book->numBuy);
			strncat(rtnMsg, buff, strlen(buff));
			count++;
			current = current->next;
		}
	}

#ifdef TEST_BRIEF
	DIAG_INFO("Msg to be briefed is:");
	printf("%s", rtnMsg);
#endif

	return rtnMsg;
}

char * briefActy(uint32_t numActy, struct actyList * activities)
{
	uint32_t count = 0U;
	struct actyList * current = activities;
	char * rtnMsg = NULL;

	DIAG_INFO("numActy = %d; activities = %#x.", numActy, (uint32_t)activities);

	rtnMsg = (char *)malloc(MAX_BRIEF_LENGTH*sizeof(char));
	if (NULL == rtnMsg)
	{
		DIAG_ERROR("Failed to allocate memory.");
	}
	else if ((0U == numActy) || (NULL == activities))
	{
		DIAG_INFO("No activities.");
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "There are no activities yet!\n");
	}
	else
	{
		while (NULL != current)
		{
			char buff[MAX_ACTY_LENGTH];
			printf("acty address: %p (%s)\n", current, current->acty);
			snprintf(buff, MAX_ACTY_LENGTH, "%s: %s\n", myAscTimeLite(&current->timestamp), current->acty);
			if (strlen(rtnMsg)+strlen(buff) >= MAX_BRIEF_LENGTH)
			{
				DIAG_WARN("rtnMsg (%dBytes) to be overflow.", strlen(rtnMsg));
				break;
			}
			strncat(rtnMsg, buff, strlen(buff));
			count++;
			current = current->next;
		}

		if (count != numActy)
		{
			DIAG_WARN("Wrong number of activities (%d %d).", count, numActy);
		}
	}

#ifdef TEST_BRIEF
	DIAG_INFO("Msg to be briefed is:");
	printf("%s", rtnMsg);
#endif

	return rtnMsg;
}

char * briefGlabalBookHty(ACTY_FILTER_TAG acty_filter)
{
	uint32_t i;
	char * rtnMsg = NULL;

	DIAG_INFO("acty_filter = %d.", acty_filter);

	rtnMsg = (char *)malloc(MAX_BRIEF_LENGTH*sizeof(char));
	if (NULL == rtnMsg)
	{
		DIAG_ERROR("Failed to allocate memory.");
	}
	if ((acty_filter < acty_filter_undef) || (acty_filter > acty_filter_lastvalue))
	{
		DIAG_ERROR("Invalid parameter.");
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "System error.\n");
	}
	else
	{
		memset(rtnMsg, 0x00, (size_t)MAX_BRIEF_LENGTH);

		for (i = g_bookTotal.actyHeadOffset; i == g_bookTotal.actyTailOffset; i = (i + 1U) % MAX_SYS_ACTIES_AMOUNT)
		{
			if (ACTY_FILTER_BY_WOW == acty_filter)
			{
				if (NULL == strstr(g_bookTotal.activities[i], "WOW")) continue;
			}
			else if (ACTY_FILTER_BY_BUY == acty_filter)
			{
				if (NULL == strstr(g_bookTotal.activities[i], "BUY")) continue;
			}
			else if (ACTY_FILTER_ALL != acty_filter)
			{
				DIAG_WARN("Invalid filter type.");
			}
			else
			{
				/* Misra. */
			}

			strncat(rtnMsg, g_bookTotal.activities[i], strlen(g_bookTotal.activities[i]));
		}
	}

#ifdef TEST_BRIEF
	DIAG_INFO("Msg to be briefed is:");
	printf("%s", rtnMsg);
#endif

	return rtnMsg;
}

char * briefMsgBox(USER_INFO * host)
{
	char * rtnMsg = NULL;

	DIAG_INFO("briefMsgBox(): starts.");
	DIAG_INFO("host = %#x", (uint32_t)host);

	rtnMsg = (char *)malloc(MAX_BRIEF_LENGTH*sizeof(char));
	if (NULL == rtnMsg)
	{
		DIAG_ERROR("Failed to allocate memory.");
	}
	else if (NULL == host)
	{
		DIAG_ERROR("Invalid parameter.");
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "System error.\n");
	}
	else if (0U == host->msgBox.numMsg)
	{
		DIAG_INFO("Empty inbox for user %s.", host->username);
		snprintf(rtnMsg, 26U, "Your msg inbox is empty.\n");
	}
	else
	{
		struct msgList * current = host->msgBox.msgs;
		uint32_t counter = 0U;

		DIAG_INFO("Msg box of %s:", host->username);
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "There are %d messages in your inbox, %d unread.\n", host->msgBox.numMsg, host->msgBox.numUnread);
		while ((NULL != current) || (counter >4U))
		{
			if (NULL == current->contexts)
			{
				DIAG_ERROR("Empty context! (user: %s; msg contact: %s)", host->username, current->contact);
			}
			else
			{
				char buf[66U];
				char msg_buf[14];

				memset(msg_buf, 0x00, 14U);
				strncpy(msg_buf, current->contexts->msg.msg, (size_t)10U);
				strncat(msg_buf, (strlen(current->contexts->msg.msg) > 10) ? "..." : "", (size_t)3U);
				/* Not sure if sprintf will append string or replace it...*/
				snprintf(buf, sizeof(buf), "        [%10s][%9s][%8s][%s]-%s\n", current->contact,
						     myAscTimeLite(&current->contexts->msg.datetime),
						     (MSG_RECEIPT_IN == current->contexts->msg.io) ? "received" : "sent",
						     (true == current->contexts->msg.unread) ? "unread" : "read",
						     msg_buf);
				printf("#%d - %s\n", counter, buf);
//				msgConvert(buf); /* What am I supposed to do here? */
				strncat(rtnMsg, buf, strlen(buf));
			}

			current = current->next;
			counter++;
		}
	}

#ifdef TEST_BRIEF
	DIAG_INFO("Msg to be briefed is:");
	printf("%s", rtnMsg);
#endif

	DIAG_INFO("briefMsgBox(): ends.");

	return rtnMsg;
}

char * briefMsgContexts(struct msgList * msgNode)
{
	uint32_t counter = 0U;
	char buf[56U];
	char * rtnMsg = NULL;

	if (NULL == msgNode)
	{
		DIAG_ERROR("Invalid parameter.");
		snprintf(rtnMsg, MAX_BRIEF_LENGTH, "System error.\n");
	}
	else
	{
		uint32_t numMsgs = msgNode->numMsg;
		uint32_t numUnread = msgNode->numUnread;
		struct msgContext * current = msgNode->contexts;

		DIAG_INFO("numMsgs = %d; numUnread = %d; context = %#x", numMsgs, numUnread, (uint32_t)current);

		rtnMsg = (char *)malloc(MAX_BRIEF_LENGTH*sizeof(char));
		if (NULL == rtnMsg)
		{
			DIAG_ERROR("Failed to allocate memory.");
		}
		else
		{
			snprintf(rtnMsg, MAX_BRIEF_LENGTH, "There are %d messages, %d unread.\n", numMsgs, numUnread);
			uint32_t lenth = 0U;
			while ((NULL != current) || (counter >4U))
			{
				snprintf(buf, sizeof(buf), "[%9s][%8s][%s]: %10s%s\n",
							 myAscTimeLite(&current->msg.datetime),
							 (MSG_RECEIPT_IN == current->msg.io) ? "received" : "sent",
							 (true == current->msg.unread) ? "unread" : "read",
							 current->msg.msg,
							 (strlen(current->msg.msg) > 11) ? "..." : "");
				printf("#%d - %s\n", counter, buf);
	lenth += strlen(buf);
	printf("length = %d (%d)\n", lenth, strlen(rtnMsg));
				strncat(rtnMsg, buf, strlen(buf));
				current = current->next;
				counter++;
			}

			msgNode->numUnread = 0U;

		}
	}
#ifdef TEST_BRIEF
	DIAG_INFO("Msg to be briefed is:");
	printf("%s", rtnMsg);
#endif

	return rtnMsg;
}
