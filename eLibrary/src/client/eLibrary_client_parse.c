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
#include "common_types.h"
#include "eLibrary_serverHandler.h"
#include "eLibrary_serverParser.h"
#include "eLibrary_serverTypes.h"
#include "eLibrary_serverBrief.h"
#include "eLibrary.h"


SYSTEM_STATUS eLibrary_SVR_input_Init(COMMON_HANDLE session_handle,
									  HANDLER_METHODS handlerCB,
									  OUTPUT_METHODS outputCB,
									  shipData_func shipDataCB)
{
	SYSTEM_STATUS result = COMMON_STATUS_FAIL;
	SESSION_DATA session_data;

	DIAG_ENTRY("eLibrary_SVR_input__Init");
	COMMON_DIAG_PARAM("session_handle %#x", session_handle);

	if (COMMON_STATUS_OK != COMMON_HDL_getObject(session_handle, &session_data))
	{
		DIAG_WARN("COMMON_HDL_retrieveObject() failed.");
		result = COMMON_STATUS_FAIL;
	}
	else
	{
		session_data.handler = handlerCB;
		session_data.output = outputCB;
		session_data.shipData = handlerCB;
	}

	DIAG_EXIT(result);
	return result;
}

SYSTEM_STATUS eLibrary_SVR_input_Term()
{
	SYSTEM_STATUS result = COMMON_STATUS_FAIL;

	DIAG_ENTRY("eLibrary_SVR_input__Term");

	result = COMMON_ITC_PostEvent(g_server_context.parser_queue, SERVER_EVT_TERMINATE, NULL, 0U);
	posix_thread_join();

	DIAG_EXIT(result);
	return result;
}



char * eLibrary_SVR_input_View(COMMON_HANDLE session_handle, cmd_line_tag * parsedCmd)
{
	SESSION_DATA *p_session_data;
	BOOK_FILTER filter;
	SORT_METHOD sortMethod = SORT_BY_DATE; /* This is the default sorting. */
	bool cmdValid = false;
	char * rtnMsg = NULL;
	USER_INFO * friend = NULL;
	USER_INFO * host_data = NULL;
	char * frdName = NULL;
	SYSTEM_STATUS result = STATUS_OK;
	bool responseParsed = false;
	BOOK_INFO *p_book_data = NULL;
	COMMON_HANDLE book_handle;

	COMMON_DIAG_ENTRY("eLibrary_SVR_input_View");
	COMMON_DIAG_PARAM("session_handle %#x", session_handle);

	if (COMMON_STATUS_OK != COMMON_HDL_getObject(session_handle, &p_session_data))
	{
		DIAG_WARN("COMMON_HDL_retrieveObject() failed.");
		result = COMMON_STATUS_FAIL;
	}
	else
	{
		result = COMMON_HDL_getObject(p_session_data->user_id, host_data);
		if (COMMON_STATUS_OK == result)
		{
			DIAG_ERROR("COMMON_HDL_getObject() failed.");
			result = COMMON_STATUS_FAIL;
		}
		else
		{
			if ((0U == parsedCmd->num_options) && (parsedCmd->num_params > 0)) /* view book_name: read a book. */
			{
				DIAG_INFO("Reading book %s", parsedCmd->params[0]);

				if (COMMON_NULL_HANDLE != p_session_data->book_handle_alive)
				{
					if (COMMON_STATUS_OK != COMMON_HDL_getObject(p_session_data->book_handle_alive, p_book_data))
					{
						DIAG_ERROR("COMMON_HDL_getObject() failed.");
						result = COMMON_STATUS_FAIL;
					}
					else if (0U != strcmp(parsedCmd->params[0], p_book_data))
					{
						DIAG_WARN("Only one book can be opened at a instance.")
					}
					else
					{}

					book_handle = p_session_data->book_handle_alive;
				}
				else
				{
					result = p_session_data->handler.findBook(parsedCmd->params[0], &book_handle);
				}

				result = p_session_data.handler.viewBook(book_handle, buf);
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
							result = COMMON_STATUS_FAIL;
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
						result = COMMON_STATUS_FAIL;
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

	session_data.sendData(rtnMsg);

	COMMON_DIAG_EXIT(result);
	return result;
}

char * eLibrary_SVR_input_Add(COMMON_HANDLE session_handle, cmd_line_tag * parsedCmd)
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
						result = COMMON_STATUS_FAIL;
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
			result = COMMON_STATUS_FAIL;
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

char * eLibrary_SVR_input_Rm(COMMON_HANDLE session_handle, cmd_line_tag * parsedCmd)
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
			result = COMMON_STATUS_FAIL;
		}
	}

	if (true != cmdValid) rtnMsg = briefEx(BRIEF_EVT_ERR_CMD_INVALID, NULL);
	if (STATUS_OK != result) rtnMsg = briefEx(BRIEF_EVT_ERR_GENERAL, NULL);

	return rtnMsg;
}

char * eLibrary_SVR_input_Search(COMMON_HANDLE session_handle, cmd_line_tag * parsedCmd)
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
				result = COMMON_STATUS_FAIL;
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

char * eLibrary_SVR_input_Whatsup(COMMON_HANDLE session_handle, cmd_line_tag * parsedCmd)
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
		result = COMMON_STATUS_FAIL;
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
			result = COMMON_STATUS_FAIL;
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

char * eLibrary_SVR_input_Wow(COMMON_HANDLE session_handle, cmd_line_tag * parsedCmd)
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
				result = COMMON_STATUS_FAIL;
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
			result = COMMON_STATUS_FAIL;
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

char * eLibrary_SVR_input_Buy(COMMON_HANDLE session_handle, cmd_line_tag * parsedCmd)
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
				result = COMMON_STATUS_FAIL;
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
			result = COMMON_STATUS_FAIL;
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

char * eLibrary_SVR_input_Msg(COMMON_HANDLE session_handle, cmd_line_tag * parsedCmd)
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
		result = COMMON_STATUS_FAIL;
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

void * eLibrary_SVR_input_Msg(void * p)
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
