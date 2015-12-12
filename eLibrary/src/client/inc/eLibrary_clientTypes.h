/*
 * eLibrary_clientTypes.h
 *
 *  Created on: 26 Aug 2013
 *      Author: singlaive
 */

#ifndef ELIBRARY_CLIENTTYPES_H_
#define ELIBRARY_CLIENTTYPES_H_

#include "common_stack.h"
#include "eLibrary_sharedTypes.h"

#define MAX_OBJ_STR_LENGTH 32U
#define MAX_ATTACHED_STR_LENGTH 99U
#define MAX_content_length 800U /* 10 lines */

typedef struct
{
	USER_ID user_id;
	uint32_t		  numFrds;
	struct userList * friends;
	uint32_t		  numBooks;
	struct bookList	* books;
	uint32_t		  numActy;
	struct actyList * activity;
}CLIENT_USER_INFO;

typedef struct
{
	uint32_t offset;
	char text[MAX_content_length]
}TEXT_CONTENT;

typedef struct
{
	uint32_t client_id;
	CLIENT_USER_INFO user_info_cache;
	STACK_HANDLE other_user_chache;
	STACK_HANDLE books_viewing_history;
	TEXT_CONTENT text_cxt;
}CLIENT_CONTEXT;

enum briefEVT
{
	brief_evt_undef = -1,
	BRIEF_EVT_FAIL,

	BRIEF_EVT_HLP_VIEW,
	BRIEF_EVT_HLP_ADD,
	BRIEF_EVT_HLP_RM,
	BRIEF_EVT_HLP_SEARCH,
	BRIEF_EVT_HLP_WHATSUP,
	BRIEF_EVT_HLP_WOW,
	BRIEF_EVT_HLP_BUY,
	BRIEF_EVT_HLP_MSG,

	BRIEF_EVT_VIEW_USER,
	BRIEF_EVT_VIEW_FRD_ALL,
	BRIEF_EVT_VIEW_BOOK,

	BRIEF_EVT_ADD_FRIEND,
	BRIEF_EVT_ADD_DESP,
	BRIEF_EVT_ADD_GROUP,
	BRIEF_EVT_ADD_BOOK,

	BRIEF_EVT_RM_FRIEND,
	BRIEF_EVT_RM_BOOK,

	BRIEF_EVT_WOW_BOOK,
	BRIEF_EVT_BUY_BOOK,

	BRIEF_EVT_ERR_CMD_INVALID,
	BRIEF_EVT_ERR_GENERAL,

	brief_evt_lastvalue,
};
#endif /* ELIBRARY_CLIENTTYPES_H_ */
