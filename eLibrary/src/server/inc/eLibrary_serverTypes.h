/*
 * eLibrary_serverTypes.h
 *
 *  Created on: 18 Oct 2012
 *      Author: singlaive
 */

#ifndef ELIBRARY_SERVERTYPES_H_
#define ELIBRARY_SERVERTYPES_H_

#include "common_types.h"

#define SERVER_EVENT_JOB 1U
#define SERVER_EVENT_TERMINATION 2U


#define MAX_ACTIES_AMOUNT 12U
#define MAX_SYS_ACTIES_AMOUNT 30U
#define MAX_BRIEF_LENGTH 1000U
#define MAX_SYS_CMD_LENGTH 50U
#define MAX_EVENT_LENGTH	2*MAX_NAME_LENGTH+MAX_MSG_LENGTH
#define MAX_NUM_WORKERS 4U
#define MAX_JOB_SPEC_LENGTH 999U
#define USER_ID_INVALID 0U

typedef COMMON_HANDLE USER_ID;


typedef struct
{
	char name[MAX_LENGTH_NAME];
	time_t date_registered;
	time_t date_last_visted;

	/********** Books ***************/
	uint32_t booksCollectedMap[MAX_NO_BOOKS_COLLECTED];
	uint32_t booksUploadedMap[MAX_NO_BOOKS_UPLOADED];
	uint32_t numBooksCollected;
	uint32_t numBooksUploaded;
	uint32_t numBooksTotal;
	/*Last viewed book*/
	/********************************/

	/********* Friends **************/
	uint32_t friendsMap[MAX_NO_FRIENDS];
	uint32_t numFriends;
	/********************************/

	/********* Activities ***********/
	uint32_t WOWedBooksMap;
	uint32_t BUYedBooksMap;
	uint32_t book_id_last_viewed;
	FILE page_last_viewed;
	/********************************/

	MSG_BOX			  msgBox;


}USER_INFO;

typedef struct
{
	char name[];
	COMMON_HANDLE uploaderId;
	uint32_t numCollected;
	/*tag*/
}BOOK_INFO;


typedef enum
{
	undef = -1,
	MSG_RECEIPT_IN,
	MSG_RECEIPT_OUT,
	lastvalue,
}MSG_DIRECTION;

typedef struct
{
	MSG_DIRECTION io;
	bool		unread;
	time_t		datetime;
	char		msg[MAX_EMAIL_LENGTH];
}MSG_FORMAT;

struct msgContext
{
	MSG_FORMAT msg;
	struct msgContext * next;
};

struct msgList
{
	char contact[MAX_NAME_LENGTH];
	uint32_t numMsg;
	uint32_t numUnread;
	struct msgContext * contexts;
	struct msgList * next;
};

typedef struct
{
	uint32_t numMsg;
	uint32_t numUnread;
	struct msgList * msgs; /* Messeges are grouped by contacts. */
}MSG_BOX;

struct userList
{
	USER_INFO * user;
	struct userList * next;
};

struct actyList
{
	char acty[MAX_ACTY_LENGTH];
	time_t timestamp;
	struct actyList * next;
};

typedef enum
{
	book_filter_tag_undef = -1,
	FILTER_BY_BOOK_NAME,
	FILTER_BY_USER,
	book_filter_tag_lastvalue,
}BOOK_FILTER_TAG;

typedef struct
{
	BOOK_FILTER_TAG tag;
	union
	{
		char * bookName;
		USER_INFO * user;
	} filter;
}BOOK_FILTER;

typedef struct
{
	char name[MAX_BOOK_NAME_LENGTH];
	char desp[MAX_DESP_LENGTH];
	BOOK_GROUP	group;
	USER_INFO * owner;
	uint32_t	numWow;
	struct nameDateList * whoWowed; /* A name list who wowed this book. */
	uint32_t	numBuy;
	struct nameDateList * whoBuyed; /* A name list who buyed this book. */
	uint32_t countCollected;
}BOOK_INFO;

struct bookList
{
	BOOK_INFO * book;
	struct bookList * next;
};

struct user_dataBase
{
	uint32_t 	      numUsers;
	struct userList * listHead;
	struct userList * listTail;
	pthread_mutex_t   lock;
};

typedef struct
{
	uint32_t retrieval_type;
	void *pay_load;
	COMMON_HANDLE conn_handle;
}JOB_MSG_DATA;


typedef struct
{
	USER_ID user_id;
	char dest_str;
	COMMON_HANDLE book_handle_alive;           /* Book that is opened currently. Only 1 book is allowed to be opened at a instance per user. */

	HANDLER_METHODS handler;
}SERVER_SESSION;

typedef struct
{
	COMMON_HANDLE queue_id;
	CNM_INTERFACE conn;
	DB_ENGINE db;
	COMMON_HANDLE job_center;
	LIST_HANDLE session_list;
}SERVER_CONTEXT;

#endif /* ELIBRARY_SERVERTYPES_H_ */
