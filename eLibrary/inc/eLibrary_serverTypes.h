/*
 * eLibrary_serverTypes.h
 *
 *  Created on: 18 Oct 2012
 *      Author: singlaive
 */

#ifndef ELIBRARY_SERVERTYPES_H_
#define ELIBRARY_SERVERTYPES_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_CW   8U
#define VIEW     0U
#define ADD		 1U
#define RM		 2U
#define SEARCH	 3U
#define WHATSUP	 4U
#define WOW		 5U
#define BUY		 6U
#define MSG		 7U

#define MAX_NAME_LENGTH 10U
#define MAX_EMAIL_LENGTH 41U
#define MAX_BOOK_NAME_LENGTH 30U
#define MAX_DESP_LENGTH 100U
#define MAX_MSG_LENGTH  30U
#define MAX_LENGTH_CONTEXTS 100U
#define MAX_ACTY_LENGTH 70U
#define MAX_ACTIES_AMOUNT 12U
#define MAX_SYS_ACTIES_AMOUNT 30U
#define MAX_BRIEF_LENGTH 1000U
#define MAX_SYS_CMD_LENGTH 50U
#define MAX_EVENT_LENGTH	2*MAX_NAME_LENGTH+MAX_MSG_LENGTH

extern pthread_mutex_t   lock_exchange;
extern pthread_cond_t    cond_exchange;

bool g_msgSent;
char g_sysEvtPayload[MAX_EVENT_LENGTH];

uint32_t g_lock_exchange_count;
uint32_t g_unlock_exchange_count;

bool g_msgExchange_ready;

typedef enum
{
	sys_evt_undef = 0,
	SYSTEM_EVENT_MSG_SENT,
	SYSTEM_EVENT_SHUTDOWN,
	sys_evt_lastvalue,
}SYS_EVT;

extern SYS_EVT g_systemEvent;

typedef struct
{
	char *cmd;
	char  option;
	void *func;
}COMMAND;

typedef enum
{
	book_group_undef = -1,
	FICTION,
	LITERATURE,
	book_group_lastValue,
}BOOK_GROUP;

typedef enum
{
	sort_method_undef = -1,
	SORT_BY_DATE,
	SORT_BY_ALFA,
	SORT_BY_GROUP,
	sort_method_lastvalue,
}SORT_METHOD;

typedef enum
{
	acty_filter_undef = -1,
	ACTY_FILTER_ALL,
	ACTY_FILTER_USER_CREATE,
	ACTY_FILTER_ADDING_BOOK,
	ACTY_FILTER_ADDING_FRD,
	ACTY_FILTER_DELETING_BOOK,
	ACTY_FILTER_BY_WOW,
	ACTY_FILTER_BY_BUY,
	acty_filter_lastvalue,
}ACTY_FILTER_TAG;

typedef struct
{
	ACTY_FILTER_TAG tag;
}ACTY_FILTER;

struct nameDateList
{
	time_t date;
	char 	 name[MAX_NAME_LENGTH];
	struct nameDateList * next;
};

typedef struct
{
	struct bookList * head;
	struct bookList * tail;
	uint32_t numBooks;
	uint32_t actyHeadOffset;		/* Pointing to the start point in the activity array. */
	uint32_t actyTailOffset;		/* Pointing to the next available slot for new entry. */
	char activities[MAX_ACTY_LENGTH][MAX_ACTY_LENGTH]; /* This is a ring array to store recently activities. Thus the beginning of the array might be overiden by later entries. */
}BOOK_TOTAL;

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

typedef struct userInfo
{
	char 			  username[MAX_NAME_LENGTH];
	char 			  email[MAX_EMAIL_LENGTH];
	uint32_t		  userId;
	uint32_t		  numFrds;
	struct userList * friends;
	uint32_t		  numBooks;
	struct bookList	* books;
	uint32_t		  numActy;
	struct actyList * lastActy;
	struct actyList * activity;
	MSG_BOX			  msgBox;
}USER_INFO;

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








pthread_mutex_t lock_userList;

char g_cmdMsg[MAX_SYS_CMD_LENGTH];


struct user_dataBase g_userData;
BOOK_TOTAL g_bookTotal;

#define LINELEN_MAX 80U

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



#endif /* ELIBRARY_SERVERTYPES_H_ */
