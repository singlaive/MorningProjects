/*
 * eLibrary_client_api.c
 *
 *  Created on: 29 Aug 2013
 *      Author: singlaive
 */


#include "common_types.h"
#include "eLibrary_sharedTypes.h"
#include "eLibrary_clientTypes.h"

CLIENT_CONTEXT f_client_context;


SYSTEM_STATUS eLibrary_CLT_registerClient()
{

	COMMON_DIAG_ENTRY("eLibrary_CLT_readCache");
	COMMON_DIAG_EXIT(result);
	return result;
}
SYSTEM_STATUS eLibrary_CLT_destroyClient()
{

	COMMON_DIAG_ENTRY("eLibrary_CLT_readCache");
	COMMON_DIAG_EXIT(result);
	return result;
}

SYSTEM_STATUS eLibrary_CLT_createRequest(const char *buf, CLIENT_REQUEST *p_request_data)
{
	SYSTEM_STATUS result = COMMON_STATUS_OK;
	cmd_line_tag abstracted_command;
	uint32_t RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_INVALID;

	COMMON_DIAG_ENTRY("eLibrary_CLT_createRequest");

	result = COMMON_parse_cmd(buf, abstracted_command);
	if (COMMON_STATUS_OK != result)
	{
		COMMON_DIAG_ERROR("Invalid parameter.");
		result = COMMON_STATUS_INVALID_PARAM;
	}
	else if (0 == strcmp("view", abstracted_command.cmd))
	{
		if ((1U == abstracted_command.num_options) && ('b' == abstracted_command.option[0]) && (0U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
			p_request_data->filter_id = FILTER_BY_USER_SELF | FILTER_BY_UPLOADED | FILTER_BY_COLLECTED;
			p_request_data->sort_id = SORT_BY_TIME;
		}
		else if ((1U == abstracted_command.num_options) && ('b' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
			p_request_data->filter_id = FILTER_BY_BOOK_NAME;
			p_request_data->sort_id = SORT_INVALID;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else if ((1U == abstracted_command.num_options) && ('f' == abstracted_command.option[0]) && (0U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_USERS;
			p_request_data->filter_id = FILTER_BY_USER_SELF;
			p_request_data->sort_id = SORT_BY_TIME;
		}
		else if ((1U == abstracted_command.num_options) && ('f' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_USERS;
			p_request_data->filter_id = FILTER_BY_USER_NAME;
			p_request_data->sort_id = SORT_INVALID;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else if ((2U == abstracted_command.num_options) && ('b' == abstracted_command.option[0]))
		{
			if (('c' == abstracted_command.option[1]) && (0U == abstracted_command.num_params))
			{
				RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
				p_request_data->filter_id = FILTER_BY_USER_SELF | FILTER_BY_COLLECTED;
				p_request_data->sort_id = SORT_BY_TIME;
			}
			else if (('t' == abstracted_command.option[1]) && (1U == abstracted_command.num_params))
			{
				RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
				p_request_data->filter_id = FILTER_BY_USER_SELF | FILTER_BY_TAG | FILTER_BY_UPLOADED | FILTER_BY_COLLECTED;
				p_request_data->sort_id = SORT_BY_TIME;
				strcpy(p_request_data->object_str, abstracted_command.params[0U])
			}
			else if (('u' == abstracted_command.option[1]) && (0U == abstracted_command.num_params))
			{
				RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
				p_request_data->filter_id = FILTER_BY_USER_SELF | FILTER_BY_UPLOADED;
				p_request_data->sort_id = SORT_BY_TIME;
			}
			else
			{
				COMMON_DIAG_ERROR("Invalid parameter.");
				result = COMMON_STATUS_INVALID_PARAM;
			}
		}
		else if ((2U == abstracted_command.num_options) && ('f' == abstracted_command.option[0]))
		{
			if (('b' == abstracted_command.option[1]) && (1U == abstracted_command.num_params))
			{
				RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
				p_request_data->filter_id = FILTER_BY_UPLOADED | FILTER_BY_USER_FRIENDLY;
				p_request_data->sort_id = SORT_BY_TIME;
				strcpy(p_request_data->object_str, abstracted_command.params[0U])
			}
			else if (('f' == abstracted_command.option[1]) && (1U == abstracted_command.num_params))
			{
				RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_USERS;
				p_request_data->filter_id = FILTER_BY_USER_NAME | FILTER_BY_USER_FRIENDLY;
				p_request_data->sort_id = SORT_BY_TIME;
				strcpy(p_request_data->object_str, abstracted_command.params[0U])
			}
			else
			{
				COMMON_DIAG_ERROR("Invalid parameter.");
				result = COMMON_STATUS_INVALID_PARAM;
			}
		}
		else
		{
			COMMON_DIAG_ERROR("Invalid parameter.");
			result = COMMON_STATUS_INVALID_PARAM;
		}
	}
	else if ((0 == strcmp("add", abstracted_command.cmd)) && (abstracted_command.num_params > 0U))
	{
		p_request_data->filter_id = FILTER_INVALID;
		p_request_data->sort_id = SORT_INVALID;

		if ((1U == abstracted_command.num_options) && ('b' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_ADD_BOOK;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else if ((1U == abstracted_command.num_options) && ('d' == abstracted_command.option[0]) && (2U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_ADD_BOOK_DESP;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
			strcpy(p_request_data->attached_str, abstracted_command.params[1U]);
		}
		else if ((1U == abstracted_command.num_options) && ('t' == abstracted_command.option[0]) && (2U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_ADD_TAG;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
			strcpy(p_request_data->attached_str, abstracted_command.params[1U]);
		}
		else if ((1U == abstracted_command.num_options) && ('c' == abstracted_command.option[0]) && (2U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_ADD_COMMENT;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
			strcpy(p_request_data->attached_str, abstracted_command.params[1U]);
		}
		else if ((1U == abstracted_command.num_options) && ('f' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_ADD_FRIEND;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else
		{
			COMMON_DIAG_ERROR("Invalid parameter.");
			result = COMMON_STATUS_INVALID_PARAM;
		}
	}
	else if (0 == strcmp("rm", abstracted_command.cmd) && (1U == abstracted_command.num_options))
	{
		if (('b' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_RM_BOOK;
			p_request_data->filter_id = FILTER_BY_BOOK_NAME;
			p_request_data->sort_id = SORT_INVALID;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else if (('t' == abstracted_command.option[0]) && (2U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_RM_TAG;
			p_request_data->filter_id = FILTER_BY_TAG;
			p_request_data->sort_id = SORT_INVALID;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else if (('f' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_RM_FRIEND;
			p_request_data->filter_id = FILTER_BY_USER_NAME;
			p_request_data->sort_id = SORT_INVALID;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else
		{
			COMMON_DIAG_ERROR("Invalid parameter.");
			result = COMMON_STATUS_INVALID_PARAM;
		}
	}
	else if ((0 == strcmp("search", abstracted_command.cmd)) && (1U == abstracted_command.num_options))
	{
		if (('b' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
			p_request_data->filter_id = FILTER_BY_BOOK_NAME;
			p_request_data->sort_id = SORT_INVALID;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else if (('t' == abstracted_command.option[0]) && (2U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
			p_request_data->filter_id = FILTER_BY_TAG;
			p_request_data->sort_id = SORT_BY_TIME;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else if (('f' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_USERS;
			p_request_data->filter_id = FILTER_BY_USER_NAME;
			p_request_data->sort_id = SORT_INVALID;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else
		{
			COMMON_DIAG_ERROR("Invalid parameter.");
			result = COMMON_STATUS_INVALID_PARAM;
		}
	}
	else if (0 == strcmp("whatsup", abstracted_command.cmd))
	{
		RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_ACTIVITIES;
		p_request_data->sort_id = SORT_BY_TIME;

		if ((0U == abstracted_command.num_params) && (0U == abstracted_command.num_params))
		{
			p_request_data->filter_id = FILTER_BY_USER_SELF | FILTER_BY_USER_FRIENDLY;
		}
		else if ((1U == abstracted_command.num_options) && ('m' == abstracted_command.option[0]) && (0U == abstracted_command.num_params))
		{
			p_request_data->filter_id = FILTER_INVALID;
			p_request_data->filter_id = FILTER_BY_USER_SELF;
		}
		else if ((1U == abstracted_command.num_options) && ('f' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			p_request_data->filter_id = FILTER_BY_USER_NAME | FILTER_BY_USER_FRIENDLY;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else
		{
			COMMON_DIAG_ERROR("Invalid parameter.");
			result = COMMON_STATUS_INVALID_PARAM;
		}
	}
	else if (0 == strcmp("wow", abstracted_command.cmd))
	{
		if ((0U == abstracted_command.num_params) && (0U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
			p_request_data->filter_id = FILTER_BY_USER_SELF | FILTER_BY_WOWED;
			p_request_data->sort_id = SORT_BY_TIME;
		}
		else if ((0U == abstracted_command.num_options) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_WOW_BOOK;
			p_request_data->filter_id = FILTER_BY_BOOK_NAME;
			p_request_data->sort_id = SORT_INVALID;
		}
		else if ((1U == abstracted_command.num_options) && ('f' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
			p_request_data->filter_id = FILTER_BY_USER_NAME | FILTER_BY_WOWED | FILTER_BY_USER_FRIENDLY;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else
		{
			COMMON_DIAG_ERROR("Invalid parameter.");
			result = COMMON_STATUS_INVALID_PARAM;
		}
	}
	else if (0 == strcmp("buy", abstracted_command.cmd))
	{
		if ((0U == abstracted_command.num_params) && (0U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
			p_request_data->filter_id = FILTER_BY_USER_SELF | FILTER_BY_BUYED;
			p_request_data->sort_id = SORT_BY_TIME;
		}
		else if ((0U == abstracted_command.num_options) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BUY_BOOK;
			p_request_data->filter_id = FILTER_BY_BOOK_NAME;
			p_request_data->sort_id = SORT_INVALID;
		}
		else if ((1U == abstracted_command.num_options) && ('f' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_BOOKS;
			p_request_data->filter_id = FILTER_BY_USER_NAME | FILTER_BY_BUYED | FILTER_BY_USER_FRIENDLY;;
			strcpy(p_request_data->object_str, abstracted_command.params[0U]);
		}
		else
		{
			COMMON_DIAG_ERROR("Invalid parameter.");
			result = COMMON_STATUS_INVALID_PARAM;
		}
	}
	else if (0 == strcmp("msg", abstracted_command.cmd))
	{
		if ((0U == abstracted_command.num_params) && (0U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_MSGS;
			p_request_data->filter_id = FILTER_INVALID;
			p_request_data->sort_id = SORT_BY_TIME;
		}
		else if ((1U == abstracted_command.num_options) && ('f' == abstracted_command.option[0]) && (1U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_BROWSE_MSGS;
			p_request_data->filter_id = FILTER_BY_USER_NAME;
			p_request_data->sort_id = SORT_BY_TIME;
		}
		else if ((0U == abstracted_command.num_options) && (2U == abstracted_command.num_params))
		{
			RETRIEVAL_TYPE_id = RETRIEVAL_TYPE_SEND_MSG;
			p_request_data->filter_id = FILTER_BY_USER_NAME;
			p_request_data->sort_id = SORT_INVALID;
		}
		else
		{
			COMMON_DIAG_ERROR("Invalid parameter.");
			result = COMMON_STATUS_INVALID_PARAM;
		}
	}
	else
	{
		COMMON_DIAG_ERROR("Invalid parameter.");
		result = COMMON_STATUS_INVALID_PARAM;
	}

	COMMON_DIAG_EXIT(result);
	return result;
}


SYSTEM_STATUS eLibrary_CLT_getResponse(const CLIENT_REQUEST *p_request_data, CLIENT_RESPONSE *p_response_data)
{
	SYSTEM_STATUS result;
#if 0
	sendServer(buf, buf_len);
	sem_timedwait(&sem, DATA_RESPONSE_TIMEOUT);

	*response = eLibrary_CLT_readResponse(); /* from shared memory or from tcp/udp */
#endif
	return result;
}

bool eLibrary_CLT_readCache(const CLIENT_REQUEST *p_request_data, CLIENT_RESPONSE *p_response_data)
{
	bool cache_hit = false;
	SYSTEM_STATUS result;

	COMMON_DIAG_ENTRY("eLibrary_CLT_readCache");

	switch (p_request_data.retrieval_type)
	{
	case RETRIEVAL_TYPE_BROWSE_BOOKS:
		break;
	case RETRIEVAL_TYPE_BROWSE_USERS:
	{
		PUBLIC_USER_INFO user_info;

		while (COMMON_STATUS_OK == result)
		{
			memset(&user_info, 0x00, sizeof(PUBLIC_USER_INFO));
			result = COMMON_STACK_getNext(f_client_context.other_user_chache, &user_info);
			if (0 == strcmp(p_request_data->object_str, user_info.user_name))
			{
				cache_hit = true;
				break;
			}
		}
	}
		break;

	case RETRIEVAL_TYPE_BROWSE_TAGS:
	case RETRIEVAL_TYPE_BROWSE_ACTIVITIES:
	case RETRIEVAL_TYPE_BROWSE_MSGS:
	default:
		COMMON_DIAG_ERROR("Unsupported retrieval type %#x", p_response_data->retrieval_type);
		break;
	}

	COMMON_DIAG_EXIT(cache_hit);
	return cache_hit;
}



SYSTEM_STATUS eLibrary_fillCache(const CLIENT_RESPONSE *p_response_data)
{
	SYSTEM_STATUS result;

	COMMON_DIAG_ENTRY("eLibrary_CLT_readCache");

	switch (p_response_data->retrieval_type)
	{
	case RETRIEVAL_TYPE_BROWSE_BOOKS:
		for (i = 0U; i < p_response_data->num_objects; i++)
		{
			result = COMMON_STACK_push(f_client_context.books_viewing_history, p_response_data->object[i].books, 0U, false);
			if (COMMON_STATUS_OK != result)
			{
				COMMON_DIAG_ERROR("COMMON_STACK_push() failed.");
				break;
			}
		}
		break;
	case RETRIEVAL_TYPE_BROWSE_USERS:
		for (i = 0U; i < p_response_data->num_objects; i++)
		{
			result = COMMON_STACK_push(f_client_context.other_user_chache, p_response_data->object[i].users, 0U, false);
			if (COMMON_STATUS_OK != result)
			{
				COMMON_DIAG_ERROR("COMMON_STACK_push() failed.");
				break;
			}
		}
		break;
	case RETRIEVAL_TYPE_BROWSE_TAGS:
	case RETRIEVAL_TYPE_BROWSE_ACTIVITIES:
	case RETRIEVAL_TYPE_BROWSE_MSGS:
	default:
		COMMON_DIAG_ERROR("Unsupported retrieval type %#x", p_response_data->retrieval_type);
		break;
	}

	COMMON_DIAG_EXIT(result);
	return result;
}

SYSTEM_STATUS eLibrary_writeOutput()
{
	SYSTEM_STATUS result;

	COMMON_DIAG_ENTRY("eLibrary_writeOutput");

	switch (response.retrieval_type)
	{
	case RESPONSE_TYPE_USERS:
		f_client_context.users_cache = response.object.users;
		eLibrary_CLT_outputUsers(response.object.users);
		break;
	case RESPONSE_TYPE_BOOKS:
		f_client_context.books_cache = response.object.books;
		eLibrary_CLT_outputBooks(response.object.users);
		break;
	case RETRIEVAL_TYPE_BROWSE_BOOKS:
	case RETRIEVAL_TYPE_BROWSE_USERS:
	case RETRIEVAL_TYPE_BROWSE_TAGS:
	case RETRIEVAL_TYPE_BROWSE_ACTIVITIES:
	case RETRIEVAL_TYPE_BROWSE_MSGS:
	default:
		COMMON_DIAG_ERROR("Unsupported retrieval type %#x", p_response_data->retrieval_type);
		break;
	}

	COMMON_DIAG_EXIT(result);
	return result;
}
