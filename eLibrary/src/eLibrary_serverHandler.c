/*
 * eLibrary_serverProcess.c

 *
 *  Created on: 18 Oct 2012
 *      Author: singlaive
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"
#include "eLibrary_serverHandler.h"
#include "eLibrary_serverProcess.h"
#include "eLibrary_serverBrief.h"
#include "eLibrary.h"

char * control_world[NUM_CW] =
{
	"view",
	"add",
	"rm",
	"search",
	"whatsup",
	"wow",
	"buy",
	"msg",
};

pthread_mutex_t lockCmd = PTHREAD_MUTEX_INITIALIZER;

char * CMD_EXPORT(char * str);
int PORT_EXPORT(char * str);
uint32_t ADDR_EXPORT(char * str);
char * CLIENTNAME_EXPORT(char * str);
SYS_EVT g_systemEvent = sys_evt_undef;

void thread_cmdHandler()
{
	SYSTEM_STATUS result = STATUS_FAIL;
	char * rtnMsg = NULL;
	char * userName = NULL;
	char * cmd = NULL;
	uint8_t clientPort = 0U;
	uint32_t clientAddr = 0U;

	for (;;)
	{
		if (0 != pthread_mutex_lock(&lockCmd))
		{
			DIAG_ERROR("pthrad_mutex_lock() failed. (%s)", strerror(errno));
			rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
		}
		else
		{
			/* After the use of g_cmdMsg, give it the value of "", thus the invalid g_cmdMsg shall have a lenght of zero. 	*/
			if (strlen(g_cmdMsg) > 0)
			{
				cmd = CMD_EXPORT(g_cmdMsg);
				if (0 == strcmp("exit", cmd))
				{
					break;
				}

				clientPort = PORT_EXPORT(g_cmdMsg);
				clientAddr = ADDR_EXPORT(g_cmdMsg);
				userName = CLIENTNAME_EXPORT(g_cmdMsg);

				/* g_cmdMsg now should purely contain the command line he just entered, that can be parsed. */
				rtnMsg = eLibrary_cmdProcess(userName, g_cmdMsg);
				if (NULL == rtnMsg)
				{
					DIAG_ERROR("eLibrary_cmdProcess failed.");
				}
			}

			memset(g_cmdMsg, 0x00, MAX_SYS_CMD_LENGTH); /* Check this can make the string of length zero. */

			if (0 != pthread_mutex_unlock(&lockCmd))
			{
				DIAG_ERROR("pthrad_mutex_lock() failed. (%s)", strerror(errno));
			}
		}

		/* Read commond from g_cmdMsg = [port][msg body]; do whatever needed to do, format the result and sand back through the proper socket. */
		rtnMsg = (NULL == rtnMsg) ? "System error.\n" : rtnMsg;
		server_msgSend(clientPort, clientAddr, rtnMsg);
		if (NULL != rtnMsg) free(rtnMsg);
	}
}

char * eLibrary_cmdProcess(char * userName, char * cmdStr)
{
	cmd_line_tag parsedCmd;
	uint32_t cw;
	char * rtnMsg = NULL;

	if (STATUS_OK != parse_cmd(cmdStr, &parsedCmd))
	{
		DIAG_ERROR("parse_cmd failed (%s).", cmdStr);
		rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);
	}
	else
	{
		for (cw = 0U; cw < NUM_CW; cw++)
		{
			if (!strcmp(parsedCmd.cmd, control_world[cw]))
			{
				break;
			}
		}

		if (NUM_CW == cw)
		{
			DIAG_WARN("Invalid command (%s).", cmdStr);
			rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
		}
		else
		{
			switch (cw)
			{
			case VIEW:
				DIAG_INFO("Call handlerView().");
				rtnMsg = handlerView(userName, &parsedCmd);
				break;

			case ADD:
				DIAG_INFO("Call handlerAdd().");
				rtnMsg = handlerAdd(userName, &parsedCmd);
				break;

			case RM:
				DIAG_INFO("Call handlerRm().");
				rtnMsg = handlerRm(userName, &parsedCmd);
				break;

			case SEARCH:
				DIAG_INFO("Call handlerSearch().");
				rtnMsg = handlerSearch(userName, &parsedCmd);
				break;

			case WHATSUP:
				DIAG_INFO("Call handlerWhatsup().");
				rtnMsg = handlerWhatsup(userName, &parsedCmd);
				break;

			case WOW:
				DIAG_INFO("Call handlerWow().");
				rtnMsg = handlerWow(userName, &parsedCmd);
				break;

			case BUY:
				DIAG_INFO("Call handlerBuy().");
				rtnMsg = handlerBuy(userName, &parsedCmd);
				break;

			case MSG:
				DIAG_INFO("Call handlerMsg().");
				rtnMsg = handlerMsg(userName, &parsedCmd);
				break;

			default:
				DIAG_ERROR("Should never be here. (cw = %d)", cw);
				break;
			}
		}
	}

	return rtnMsg;
}

SYSTEM_STATUS server_msgSend(char * cmdMsg, char * rstMsg)
{
	SYSTEM_STATUS result = STATUS_FAIL;

	return result;
}

char * handlerView(char * userName, cmd_line_tag * parsedCmd)
{
	BOOK_FILTER filter;
	SORT_METHOD sortMethod = SORT_BY_DATE; /* This is the default sorting. */
	bool cmdValid = false;
	char * rtnMsg = NULL;
	USER_INFO * friend = NULL;
	USER_INFO * host = NULL;
	char * frdName = NULL;
	SYSTEM_STATUS result = STATUS_OK;
	bool responseParsed = false;

	if (0 == parsedCmd->num_options)
	{
		DIAG_WARN("Invalid command (%s).", parsedCmd->str);
		result = STATUS_FAIL;
	}
	else
	{
		host = getUserInfo(userName, NULL);
		if (NULL == host)
		{
			DIAG_ERROR("getUserInfo() failed.");
			result = STATUS_FAIL;
		}
		else
		{
			if ((0U == parsedCmd->num_options) && (parsedCmd->num_params > 0)) /* view book_name: read a book. */
			{
				DIAG_INFO("Reading book %s", parsedCmd->params[0]);
				rtnMsg = viewBook(parsedCmd->params[0]);
				responseParsed = true;
			}
			else if ((parsedCmd->num_options > 0) && ('f' == parsedCmd->option[0]))
			{
				if (1U == parsedCmd->num_options)
				{
					if (parsedCmd->num_params > 0) /* view -f frd_name: view a brief report for a given friend.*/
					{
						frdName = parsedCmd->params[0];
						friend = getFrdInfo(host, frdName);
						if (NULL == friend)
						{
							DIAG_ERROR("getFrdInfo() failed.");
							result = STATUS_FAIL;
						}
						else
						{
							rtnMsg = briefEx(BRIEF_EVT_VIEW_USER, friend);
							responseParsed = true;
						}
					}
					/* For eles clause, it means: view -f: list all friends of the current user. */

					cmdValid = true;
				}
				else if (('f' == parsedCmd->option[1]) && (parsedCmd->num_params > 0)) /* view -f -f frd_name: list all friends of a given friend. */
				{
					frdName = parsedCmd->params[0];
					host = getUserInfo(frdName, NULL);
					if (NULL == host)
					{
						DIAG_ERROR("getUserInfo() failed.");
						result = STATUS_FAIL;
					}
					else
					{
						cmdValid = true;
					}
				}
				else
				{
					/* error */
				}

				if ((true == cmdValid) && (STATUS_OK == result) && (true != responseParsed))
				{
					rtnMsg = briefEx(BRIEF_EVT_VIEW_FRD_ALL, host);
					responseParsed = true;
				}
			}

			if ((true != responseParsed) && (true == checkOpt(parsedCmd, 'b')))
			{
				uint32_t numBooks;
				struct bookList * books = NULL;

				filter.tag = FILTER_BY_USER;
				filter.filter.user = host;

				if ((true == checkOpt(parsedCmd, 'f')) && (parsedCmd->num_params > 0))
				{
					filter.filter.user = getUserInfo(parsedCmd->params[0], NULL);
				}

				if (true == checkOpt(parsedCmd, 'a'))
				{
					sortMethod = SORT_BY_ALFA;
				}
				else if (true == checkOpt(parsedCmd, 'g'))
				{
					sortMethod = SORT_BY_GROUP;
				}
				else
				{
					sortMethod = SORT_BY_DATE;
				}

				cmdValid = true;

				if (STATUS_OK != getBookInfo(&filter, sortMethod, &numBooks, &books))
				{
					DIAG_ERROR("getBookInfo() failed.");
					rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);
				}
				else
				{
					rtnMsg = briefBooks(numBooks, books);
					responseParsed = true;
				}
			}

			if ('h' == parsedCmd->option[0])
			{
				DIAG_INFO("Showing help.");
				rtnMsg = briefEx(BRIEF_EVT_HLP_MSG, NULL);
				cmdValid = true;
			}

			if (true != cmdValid) rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
			if (STATUS_OK != result) rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);
		}
	}
	return rtnMsg;
}

char * handlerAdd(char * userName, cmd_line_tag * parsedCmd)
{
	bool cmdValid = true;
	char * rtnMsg = NULL;
	SYSTEM_STATUS result = STATUS_OK;
	USER_INFO * host = NULL;

	if (0 != parsedCmd->num_options)
	{
		host = getUserInfo(userName, NULL);
		if (NULL != host)
		{
			char * desp = NULL;
			BOOK_GROUP group = book_group_undef;
			char * str = NULL;
			enum briefEVT event = brief_evt_undef;

			switch (parsedCmd->option[0])
			{
			case 'f':	/* add frd_name: add somebody as your friend. */
				DIAG_INFO("Adding friend.");
				if (parsedCmd->num_params > 0)
				{
					USER_INFO * friend = NULL;

					friend = getUserInfo(parsedCmd->params[0], NULL);
					if (NULL != friend)
					{
						result = addFriend(host, friend);
						if (STATUS_OK == result)
						{
							event = BRIEF_EVT_ADD_FRIEND;
							str = parsedCmd->params[0];
						}
						else
						{
							DIAG_ERROR("addFriend() failed. (%#x)", result);
						}
					}
					else
					{
						result = STATUS_FAIL;
						DIAG_ERROR("No user with name %s.", userName);
					}
				}
				else
				{
					cmdValid = false;
				}

				break;

			case 'd': /* add -d book_name book_desp: add description to an existing book. */
				DIAG_INFO("Adding description.");
				if (parsedCmd->num_params > 1)
				{
					desp = parsedCmd->params[1];
					str = parsedCmd->params[1];
					event = BRIEF_EVT_ADD_DESP;
				}
				else
				{
					cmdValid = false;
				}

				break;

			case 'g': /* add -g book_name book_group: add group to an existing book. */
				DIAG_INFO("Adding group.");
				if (parsedCmd->num_params > 1)
				{
					group = groupConverer(parsedCmd->params[1]);
					str = parsedCmd->params[1];
					event = BRIEF_EVT_ADD_GROUP;
				}
				else
				{
					cmdValid = false;
				}

				break;

			case 'b': /* add -b book_name: add a book, it either exist or will be uploaded now. */
				DIAG_INFO("Adding book.");
				event = BRIEF_EVT_ADD_BOOK;
				break;

			case 'h': /* add -h: show help*/
				DIAG_INFO("Showing help.")
				rtnMsg = briefEx(BRIEF_EVT_HLP_BUY, NULL);
				break;

			default:
				DIAG_WARN("Should not be here.");
				cmdValid = false;
				break;
			}

			if (brief_evt_undef != event) /* event won't be assigned if result is not OK. */
			{
				if (BRIEF_EVT_ADD_FRIEND != event)
				{
					result = addBook(host, parsedCmd->params[0], desp, group);
					if (STATUS_OK != result)
					{
						DIAG_ERROR("addBook failed.");
					}
				}

				if (STATUS_OK == result)
				{
					rtnMsg = briefEx(event, str);
				}
			}

		}
		else
		{
			result = STATUS_FAIL;
			DIAG_ERROR("No user with name %s.", userName);
		}
	}
	else
	{
		cmdValid = false;
	}

	if (true != cmdValid) rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
	if (STATUS_OK != result) rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);

	return rtnMsg;
}

char * handlerRm(char * userName, cmd_line_tag * parsedCmd)
{
	bool cmdValid = true;
	USER_INFO * host = NULL;
	char * rtnMsg = NULL;
	SYSTEM_STATUS result = STATUS_OK;
	enum briefEVT event = brief_evt_undef;
	char * target = NULL;

	if ((0 == parsedCmd->num_options) || (0 == parsedCmd->num_params))
	{
		cmdValid = false;
	}
	else
	{
		host = getUserInfo(userName, NULL);
		if (NULL != host)
		{
			target = parsedCmd->params[0];

			if ('b' == parsedCmd->option[0]) /* rm -b book_name: remove a given book */
			{
				DIAG_INFO("Removing a book.")
				result = rmBook(host, target);
				if (STATUS_OK == result)
				{
					event = BRIEF_EVT_RM_BOOK;
				}
				else
				{
					DIAG_ERROR("rmBook() failed. (%#x)", result);
				}
			}
			else if ('f' == parsedCmd->option[0]) /* rm -f frd_name: remove  given friend */
			{
				DIAG_INFO("Removing a friend.")
				result = rmFriend(host, target);
				if (STATUS_OK == result)
				{
					event = BRIEF_EVT_RM_BOOK;
				}
				else
				{
					DIAG_ERROR("rmBook() failed. (%#x)", result);
				}
			}
			else if ('h' == parsedCmd->option[0]) /* rm -h: help*/
			{
				DIAG_INFO("Showing help.")
				rtnMsg = briefEx(BRIEF_EVT_HLP_RM, NULL);
			}
			else
			{
				cmdValid = false;
			}

			if (brief_evt_undef != event) /* event won't be assigned if result is not OK. */
			{
				rtnMsg = briefEx(event, NULL);
			}
		}
		else
		{
			DIAG_ERROR("Can no find user of name %s.", userName);
			result = STATUS_FAIL;
		}
	}

	if (true != cmdValid) rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
	if (STATUS_OK != result) rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);

	return rtnMsg;
}

char * handlerSearch(char * userName, cmd_line_tag * parsedCmd)
{
	BOOK_FILTER filter;
	char * rtnMsg = NULL;
	bool cmdValid = true;
	SYSTEM_STATUS result = STATUS_OK;
	enum briefEVT event = brief_evt_undef;

	if ((0 == parsedCmd->num_options) || (0 == parsedCmd->num_params))
	{
		cmdValid = false;
	}
	else
	{
		char * target = parsedCmd->params[0];

		if ('b' == parsedCmd->option[0]) /* search -b bookname: search a book */
		{
			struct bookList * books =  NULL;
			uint32_t numBooks = 0U;

			DIAG_INFO("Searching a book %s.", target);

			filter.tag = FILTER_BY_BOOK_NAME;
			filter.filter.bookName = target;

			result = getBookInfo(&filter, sort_method_undef, &numBooks, &books);
			if (STATUS_OK == result)
			{
				event = BRIEF_EVT_VIEW_BOOK;
			}
			else
			{
				DIAG_ERROR("getBookInfo() failed. (%#x)", result);
			}
		}
		else if ('f' == parsedCmd->option[0]) /* search -f username: search a user */
		{
			USER_INFO * user = NULL;
			DIAG_INFO("Searching a person %s.", target);

			user = getUserInfo(target, NULL);
			if (NULL != user)
			{
				event = BRIEF_EVT_VIEW_USER;
			}
			else
			{
				result = STATUS_FAIL;
				DIAG_ERROR("No user of name %s.", target);
			}
		}
		else if ('h' == parsedCmd->option[0])
		{
			DIAG_INFO("Showing help.");
			rtnMsg = briefEx(BRIEF_EVT_HLP_SEARCH, NULL);
		}
		else
		{
			cmdValid = false;
		}
	}

	if (true != cmdValid) rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
	if (STATUS_OK != result) rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);

	return rtnMsg;
}

char * handlerWhatsup(char * userName, cmd_line_tag * parsedCmd)
{
	USER_INFO * host = NULL;
	struct actyList * activities = NULL;
	uint32_t numActy = 0U;
	bool cmdValid = true;
	char * rtnMsg = NULL;
	SYSTEM_STATUS result = STATUS_OK;
	bool queryParsed = false;

	host = getUserInfo(userName, NULL);
	if (NULL == host)
	{
		result = STATUS_FAIL;
		DIAG_ERROR("No user named %s.", userName);
	}
	else if (0 == parsedCmd->num_options) /* whatsup: see what happened recently. */
	{
		struct userList * current = host->friends;
		struct actyList * activities = NULL;

		DIAG_INFO("What's up all.");

		result = mergeActy(host, activities, &numActy);
		if (STATUS_OK != result)
		{
			DIAG_ERROR("mergeActy() failed. (%#x)", result);
		}
	}
	else if (('f' == parsedCmd->option[0]) && (parsedCmd->num_params > 0)) /* whatsup -f friendname: see what a friend did. */
	{
		USER_INFO * friend = NULL;

		DIAG_INFO("What's up for friend %d.", parsedCmd->num_params);

		friend = getFrdInfo(host, parsedCmd->params[0]);
		if (NULL == friend)
		{
			result = STATUS_FAIL;
			DIAG_ERROR("No friend of name %s.", parsedCmd->params[0])
		}
		else
		{
			ACTY_FILTER_TAG filter = ACTY_FILTER_ALL;

			result = getUserActy(friend, filter, &numActy, &activities);/*This shall return a number of activities by a given id. The number is configurable. */
			if (STATUS_OK != result)
			{
				DIAG_ERROR("getUserActy() failed. (%#x)", result);
			}
		}
	}
	else if ('h' == parsedCmd->option[0])
	{
		DIAG_INFO("Showing help.");
		rtnMsg = briefEx(BRIEF_EVT_HLP_WHATSUP, NULL);
		queryParsed = true;
	}
	else
	{
		cmdValid = false;
	}

	if (true != cmdValid)
	{
		rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
		DIAG_ERROR("Cmd used: %s", parsedCmd->str);
	}
	else if (STATUS_OK == result)
	{
		rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);
	}
	else if (false == queryParsed) /* Meanning we are not doing: whatsup -h*/
	{
		rtnMsg = briefActy(numActy, activities);
	}
	else
	{
		DIAG_ERROR("Shold never be here.");
	}

	return rtnMsg;
}

char * handlerWow(char * userName, cmd_line_tag * parsedCmd)
{
	struct actyList * activities = NULL;
	uint32_t numActy = 0U;
	BOOK_FILTER filter;
	struct bookList * books;
	uint32_t numbooks;
	BOOK_INFO * book = NULL;
	bool cmdValid = true;
	char * rtnMsg = NULL;
	SYSTEM_STATUS result = STATUS_OK;
	bool queryParsed = false;

	if (0 == parsedCmd->num_options)
	{
		if (0 == parsedCmd->num_params) /* wow: list recent filtered history of the whole site. */
		{
			DIAG_INFO("List all activities.");
			rtnMsg = briefGlabalBookHty(ACTY_FILTER_BY_WOW);
		}
		else /* wow bookname: tag a book by WOW. */
		{
			USER_INFO * user = NULL;

			DIAG_INFO("WOW a book %s.", parsedCmd->params[0]);

			user = getUserInfo(parsedCmd->params[0], NULL);
			if (NULL != user)
			{
				result = getBookInfo(&filter, sort_method_undef, &numbooks, &books);
				if (STATUS_OK == result)
				{
					result = wowBook(user, books->book);
					if (STATUS_OK != result)
					{
						DIAG_ERROR("wowBook() failed. (%#x)", result);
					}
					else
					{
						rtnMsg = briefEx(BRIEF_EVT_WOW_BOOK, parsedCmd->params[0]);
						queryParsed = true;
					}
				}
			}
			else
			{
				DIAG_ERROR("No user of name %s.", user->username);
				result = STATUS_FAIL;
			}
		}
	}
	else if ('f' == parsedCmd->option[0]) /* wow -f friendname: list the books this friend recently WOWed. */
	{
		USER_INFO * user = NULL;

		DIAG_INFO("Which books WOWed by %s", parsedCmd->params[0]);

		user = getUserInfo(parsedCmd->params[0], NULL);
		if (NULL != user)
		{
			result = getUserActy(user, ACTY_FILTER_BY_WOW, &numActy, &activities);
			if (STATUS_OK == result)
			{
				rtnMsg = briefActy(numActy, activities);
				queryParsed = true;
			}
			else
			{
				DIAG_ERROR("getUserInfo() failed. (%#x)", result);
			}
		}
		else
		{
			DIAG_ERROR("No user of name %s.", parsedCmd->params[0]);
			result = STATUS_FAIL;
		}
	}
	else if ('b' == parsedCmd->option[0]) /* wow -b bookname: list who WOWed this book. */
	{
		DIAG_INFO("Who WOWed this book %s", parsedCmd->params[0]);

		filter.tag = FILTER_BY_BOOK_NAME;
		filter.filter.bookName = parsedCmd->params[0];

		result = getBookInfo(&filter, sort_method_undef, &numbooks, &books);
		if (STATUS_OK == result)
		{
			result = getBookHty(books->book, ACTY_FILTER_BY_WOW, &numActy, &activities);
			if (STATUS_OK == result)
			{
				rtnMsg = briefActy(numActy, activities);
				queryParsed = true;
			}
			else
			{
				DIAG_ERROR("getUserInfo() failed. (%#x)", result);
			}
		}
		else
		{
			DIAG_ERROR("getUserInfo() failed. (%#x)", result);
		}
	}
	else if ('h' == parsedCmd->option[0])
	{
		DIAG_INFO("Showing help.");
		rtnMsg = briefEx(BRIEF_EVT_HLP_WOW, NULL);
		queryParsed = true;
	}
	else
	{
		cmdValid = false;
	}

	if (true != cmdValid)
	{
		rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
		DIAG_ERROR("Cmd used: %s", parsedCmd->str);
	}
	else if (STATUS_OK == result)
	{
		rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);
	}
	else if (false == queryParsed)
	{
		rtnMsg = briefActy(numActy, activities);
	}
	else
	{
		DIAG_ERROR("Shold never be here.");
	}

	return rtnMsg;
}

char * handlerBuy(char * userName, cmd_line_tag * parsedCmd)
{
	struct actyList * activities = NULL;
	uint32_t numActy = 0U;
	BOOK_FILTER filter;
	struct bookList * books;
	uint32_t numbooks;
	BOOK_INFO * book = NULL;
	bool cmdValid = true;
	char * rtnMsg = NULL;
	SYSTEM_STATUS result = STATUS_OK;
	bool queryParsed = false;

	if (0 == parsedCmd->num_options)
	{
		if (0 == parsedCmd->num_params) /* buy: list recent filtered history of the whole site. */
		{
			DIAG_INFO("List all activities.");
			rtnMsg = briefGlabalBookHty(ACTY_FILTER_BY_BUY);
			queryParsed = true;
		}
		else /* buy bookname: tag a book by BUY. */
		{
			USER_INFO * user = NULL;

			DIAG_INFO("BUY a book %s.", parsedCmd->params[0]);

			user = getUserInfo(userName, NULL);
			if (NULL != user)
			{
				filter.tag = FILTER_BY_BOOK_NAME;
				filter.filter.bookName = parsedCmd->params[0];

				result = getBookInfo(&filter, sort_method_undef, &numbooks, &books);
				if (STATUS_OK == result)
				{
					result = buyBook(user, books->book);
					if (STATUS_OK != result)
					{
						DIAG_ERROR("buyBook() failed. (%#x)", result);
					}
					else
					{
						rtnMsg = briefEx(BRIEF_EVT_BUY_BOOK, parsedCmd->params[0]);
						queryParsed = true;
					}
				}
			}
			else
			{
				DIAG_ERROR("No user of name %s.", user->username);
				result = STATUS_FAIL;
			}
		}
	}
	else if ('f' == parsedCmd->option[0]) /* buy -f friendname: list the books this friend recently BUYed. */
	{
		USER_INFO * user = NULL;

		DIAG_INFO("Which books BUYed by %s", parsedCmd->params[0]);

		user = getUserInfo(parsedCmd->params[0], NULL);
		if (NULL != user)
		{
			result = getUserActy(user, ACTY_FILTER_BY_BUY, &numActy, &activities);
			if (STATUS_OK == result)
			{
				rtnMsg = briefActy(numActy, activities);
				queryParsed = true;
			}
			else
			{
				DIAG_ERROR("getUserInfo() failed. (%#x)", result);
			}
		}
		else
		{
			DIAG_ERROR("No user of name %s.", parsedCmd->params[0]);
			result = STATUS_FAIL;
		}
	}
	else if ('b' == parsedCmd->option[0]) /* buy -b bookname: list who BUYed this book. */
	{
		DIAG_INFO("Who BUYed this book %s", parsedCmd->params[0]);

		filter.tag = FILTER_BY_BOOK_NAME;
		filter.filter.bookName = parsedCmd->params[0];

		result = getBookInfo(&filter, sort_method_undef, &numbooks, &books);
		if (STATUS_OK == result)
		{
			result = getBookHty(books->book, ACTY_FILTER_BY_BUY, &numActy, &activities);
			if (STATUS_OK == result)
			{
				rtnMsg = briefActy(numActy, activities);
				queryParsed = true;
			}
			else
			{
				DIAG_ERROR("getUserInfo() failed. (%#x)", result);
			}
		}
		else
		{
			DIAG_ERROR("getUserInfo() failed. (%#x)", result);
		}
	}
	else if ('h' == parsedCmd->option[0])
	{
		DIAG_INFO("Showing help.");
		rtnMsg = briefEx(BRIEF_EVT_HLP_BUY, NULL);
		queryParsed = true;
	}
	else
	{
		cmdValid = false;
	}

	if (true != cmdValid)
	{
		rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
		DIAG_ERROR("Cmd used: %s", parsedCmd->str);
	}
	else if (STATUS_OK == result)
	{
		rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);
	}
	else if (false == queryParsed)
	{
		rtnMsg = briefActy(numActy, activities);
	}
	else
	{
		DIAG_ERROR("Shold never be here.");
	}

	return rtnMsg;
}

char * handlerMsg(char * userName, cmd_line_tag * parsedCmd)
{
	uint32_t numMsgs = 0U;
	uint32_t numUnread = 0U;
	struct msgContext * contexts = NULL;
	char msgs[MAX_LENGTH_CONTEXTS];
	char buf[MAX_PAYLOAD_SIZE];
	USER_INFO * host = NULL;
	char * rtnMsg = NULL;
	bool cmdValid = true;
	SYSTEM_STATUS result = STATUS_OK;
	struct msgList * msgNode = NULL;

	host = getUserInfo(userName, NULL);
	if (NULL == host)
	{
		DIAG_ERROR("No user of name %s.", userName);
		result = STATUS_FAIL;
	}
	else if (0 == parsedCmd->num_options)
	{
		/* msg: Tell me how many msgs in total; how many unread; who sent me msgs... */
		rtnMsg = briefMsgBox(host);
	}
	else if ('f' == parsedCmd->option[0])
	{
		/* Show me all msgs about this friend. */
		if (parsedCmd->num_params > 0)
		{
			uint32_t numUnread = 0U;

			result = getMsgInfo(host, parsedCmd->params[0], &msgNode);
			if (STATUS_OK != result)
			{
				DIAG_ERROR("getMsgInfo() failed. (%#x)", result);
			}
			else
			{
				numUnread = msgNode->numUnread;
				rtnMsg = briefMsgContexts(msgNode);
				host->msgBox.numUnread -= numUnread;
			}
		}
		else
		{
			cmdValid = false;
		}
	}
	else if ('s' == parsedCmd->option[0])
	{
		if (parsedCmd->num_params > 1)
		{
			sendReceiveMsg(host, parsedCmd->params[0], parsedCmd->params[1], MSG_RECEIPT_OUT);
		}
		else
		{
			cmdValid = false;
		}
	}
	else if ('h' == parsedCmd->option[0])
	{
		rtnMsg = briefEx(BRIEF_EVT_HLP_MSG, NULL);
	}
	else
	{
		cmdValid = false;
	}

	if (true != cmdValid) rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
	if (STATUS_OK != result) rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);

	return rtnMsg;
}

void * thread_processMsg(void * p)
{
	bool msg_received = false;

	DIAG_INFO("thread_processMsg(): starts.");
	g_msgExchange_ready = true;

	if (0 == pthread_mutex_lock(&lock_exchange))
	{
		g_lock_exchange_count++;
		g_msgSent = false;

		DIAG_INFO("Current sys event = %d", g_systemEvent);
		for (;;)
		{

			while (g_systemEvent == sys_evt_undef)
			{
				DIAG_INFO("Hanging for new msg.");

				if (0 != pthread_cond_wait(&cond_exchange, &lock_exchange))
				{
					DIAG_ERROR("pthread_cond_wait failed.\n");
				}

				DIAG_INFO("Woke up from condition waiting.");
			}

			if (g_systemEvent != sys_evt_undef)
			{
				DIAG_INFO("Received an system event (%d).", g_systemEvent);

				if (SYSTEM_EVENT_SHUTDOWN == g_systemEvent)
				{
					DIAG_INFO("Msg exchange is shutting down.");
					break;
				}
				else if (SYSTEM_EVENT_MSG_SENT == g_systemEvent)
				{
					char username[MAX_NAME_LENGTH];
					char frdName[MAX_NAME_LENGTH];
					char msg[MAX_MSG_LENGTH];
					USER_INFO * host = NULL;

					memcpy(frdName, g_sysEvtPayload, (size_t)MAX_NAME_LENGTH);
					memcpy(username, g_sysEvtPayload+MAX_NAME_LENGTH, (size_t)MAX_NAME_LENGTH);
					memcpy(msg, g_sysEvtPayload+2*MAX_NAME_LENGTH, MAX_MSG_LENGTH);

					DIAG_INFO("System event (username %s; frdName %s; msg %s)", username, frdName, msg);

					host = getUserInfo(username, NULL);
					if (NULL == host)
					{
						DIAG_ERROR("No user of name %s.", username);
					}
					else
					{
						/*lock*/
						if (STATUS_OK != sendReceiveMsg(host, frdName, msg, MSG_RECEIPT_IN))
						{
							DIAG_ERROR("writeMsgInfo() failed.");
						}
						/*unlock*/
					}

				}
				else
				{
					DIAG_ERROR("Unsupported system event %d.", g_systemEvent);
				}

				g_systemEvent = sys_evt_undef;
			}

			g_msgSent = false;
		}

		if (0 !=pthread_mutex_unlock(&lock_exchange))
		{
			DIAG_ERROR("pthread_mutex_unlock failed.");
		}
		g_unlock_exchange_count++;
		DIAG_INFO("g_lock_exchange_count = %d", g_lock_exchange_count);
	}

	DIAG_INFO("thread_processMsg(): ends.");
    pthread_exit(NULL);
}

void actyFree(ACTY_FILTER_TAG filter, struct actyList * activities, uint32_t counter)
{
	switch (filter)
	{
		case ACTY_FILTER_ALL:
			COMMON_free(activities);
			break;
		case ACTY_FILTER_BY_WOW:
		case ACTY_FILTER_BY_BUY:
		{
			uint32_t i = 0U;
			struct actyList * current = NULL;
			struct actyList * cached = NULL;

			while (NULL != current)
			{
				cached = current;
				current = current->next;
				COMMON_free(cached);
				i++;
			}

			if (i != counter)
			{
				DIAG_ERROR("Error in counter of activities.")
			}
		}
			break;

		default:
			break;
	}
}

char * CMD_EXPORT(char * str)
{}
int PORT_EXPORT(char * str)
{}
uint32_t ADDR_EXPORT(char * str)
{}
char * CLIENTNAME_EXPORT(char * str)
{}
