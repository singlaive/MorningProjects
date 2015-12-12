/*
 * eLibrary_sharedTypes.h
 *
 *  Created on: 19 Aug 2013
 *      Author: singlaive
 */

#ifndef ELIBRARY_SHAREDTYPES_H_
#define ELIBRARY_SHAREDTYPES_H_

#define ELIBRARY_FILTER 	  0xB0000000
#define ELIBRARY_SORT  		  0xD0000000

typedef enum
{
	RETRIEVAL_TYPE_invalid = 0,
	RETRIEVAL_TYPE_CREATE_USER,
	RETRIEVAL_TYPE_BROWSE_BOOKS,
	RETRIEVAL_TYPE_BROWSE_USERS,
	RETRIEVAL_TYPE_BROWSE_TAGS,
	RETRIEVAL_TYPE_BROWSE_ACTIVITIES,
	RETRIEVAL_TYPE_BROWSE_MSGS,
	RETRIEVAL_TYPE_READ_BOOK,
	RETRIEVAL_TYPE_COLLECT_BOOK,
	RETRIEVAL_TYPE_ADD_BOOK,
	RETRIEVAL_TYPE_ADD_BOOK_DESP,
	RETRIEVAL_TYPE_ADD_COMMENT,
	RETRIEVAL_TYPE_ADD_TAG,
	RETRIEVAL_TYPE_ADD_USER,
	RETRIEVAL_TYPE_ADD_FRIEND,
	RETRIEVAL_TYPE_RM_BOOK,
	RETRIEVAL_TYPE_RM_TAG,
	RETRIEVAL_TYPE_RM_FRIEND,
	RETRIEVAL_TYPE_WOW_BOOK,
	RETRIEVAL_TYPE_BUY_BOOK,
	RETRIEVAL_TYPE_SEND_MSG,
	RETRIEVAL_TYPE_WORKER_TERM = 99,
	RETRIEVAL_TYPE_lastValue
}RETRIEVAL_TYPE;

#define FILTER_INVALID			(ELIBRARY_FILTER | 0xFFFFFFFU)
#define FILTER_BY_OBJ_MASK		(ELIBRARY_FILTER | 0x000000FU)
#define FILTER_BY_BOOK_NAME		(ELIBRARY_FILTER | 0x0000001U)
#define FILTER_BY_TAG			(ELIBRARY_FILTER | 0x0000002U)

#define FILTER_BY_USER_MASK		(ELIBRARY_FILTER | 0x00000F0U)
#define FILTER_BY_USER_NAME		(ELIBRARY_FILTER | 0x0000010U)
#define FILTER_BY_USER_SELF		(ELIBRARY_FILTER | 0x0000020U)
#define FILTER_BY_USER_FRIENDLY	(ELIBRARY_FILTER | 0x0000040U)

#define FILTER_BY_ATTR_MASK		(ELIBRARY_FILTER | 0x0000F00U)
#define FILTER_BY_COLLECTED		(ELIBRARY_FILTER | 0x0000100U)
#define FILTER_BY_UPLOADED		(ELIBRARY_FILTER | 0x0000200U)
#define FILTER_BY_WOWED			(ELIBRARY_FILTER | 0x0000400U)
#define FILTER_BY_BUYED			(ELIBRARY_FILTER | 0x0000800U)

#define SORT_INVALID			(ELIBRARY_SORT | 0xFFFFFFF)
#define SORT_BY_ALFA			(ELIBRARY_SORT | 0x1U)
#define SORT_BY_TIME			(ELIBRARY_SORT | 0x2U)

#define MAX_CMD_LENGTH 99U
#define MAX_NAME_LENGTH 10U
#define MAX_BOOK_NAME_LENGTH 30U
#define MAX_DESP_LENGTH 100U
#define MAX_MSG_LENGTH  30U
#define MAX_LENGTH_CONTEXTS 100U
#define MAX_ACTY_LENGTH 70U
#define MAX_TAG_NAME_LENGTH 15U

 /* local commands:  view: list all bookes on shelf; list friend
 * 						-b : show a list of all books on shelf, by default it is sorted by date
 * 						-b string: show a given book
 * 						-b -a :show all books in alphabetical order
 * 						-b -s string: show books online for a given genre
 * 						-f show a list of all freiends
 * 						-f string: show a brief report of this friend
 * 						-f -b string: show books updated by a given friend
 * 						-f -f string: show all friends of a given friend
 * 					add: add one book online, add description, add genre, add a friend, add a book from others
 * 						-b string: add a book by name, search online database first, if not, add from local
 * 						-d string string: add a description to a book
 * 						-t string string: add a tag to a book
 * 						-c string: add comments to a book
 * 						-f string: add a friend
 * 					rm: remove one book offline, or a friend
 * 						-b string: remove a book offline by name (if no one want the book, the owner shall be admin)
 * 						-t string string: remove a tag from a given book
 * 						-f string: remove a friend
 * 					search: serch people, or books
 *						-b string: search a book by name, it maches the exact result
 *						-b -t string: list all books with given tag, in time by default
 *						-f string: search a friend by name
 * 					whatsup: see what books friends are reading, what activity they recently did, can specify a friend
 * 						- : list recent activity by friends and the user himself
 * 						- f string: list a friend's recent activity
 * 						- m: list activity the user itself
 * 					wow: wow any book, list all wows recently, list wows for a specific book or from a specific friend
 * 						wow string: wow a book by name, the return result contains the wow no; one user can not wow the same book twice
 * 						wow: without any option or parameters, it list recent wowed books.
 * 						-f: list all wowed books by a friend, sorted by time
 * 						-b string: show the wow info for a given book without wowing it firstly	 *
 * 					buy: the same as wow	 *
 * 					msg: leave msg to a friend, show context if not very difficult, receive commercial msg *
 * 						- : by default it list all msg in context pattern
 * 						-s string string: send a friend a msg
 * 						-f string: filter inbox by friend name
#endif /* ELIBRARY_SHAREDTYPES_H_ */

typedef struct
{
	RETRIEVAL_TYPE retrieval_type;
	uint32_t filter_id;
	uint32_t sort_id;
	char object_str[MAX_OBJ_STR_LENGTH]; /* Which is either a book name or a user name */
	char attached_str[MAX_ATTACHED_STR_LENGTH]; /* Whitch can be a description, a tag or a comment for a book. */
}CLIENT_REQUEST;

typedef enum
{
	SERVER_RESPONSE_STATUS_undef = 0,
	SERVER_RESPONSE_STATUS_OK,
	SERVER_RESPONSE_STATUS_FAIL,
	SERVER_RESPONSE_STATUS_lastvalue
}SERVER_RESPONSE_STATUS;

typedef struct
{
	RETRIEVAL_TYPE retrieval_type;
	union
	{
		struct
		{
			uint32_t num_objects;
			union
			{
				PUBLIC_USER_INFO *users;
				PUBLIC_BOOK_INFO *books;
				char tag_name[MAX_TAG_NAME_LENGTH];
				char activitiy[MAX_ACTY_LENGTH];
				char msgs[MAX_MSG_LENGTH];
			} object;
		} content;

		SERVER_RESPONSE_STATUS response_status;
	} respond;
}SERVER_RESPONSE;

typedef struct
{
	USER_ID user_id;
	char user_name[MAX_NAME_LENGTH];
	uint32_t numFrds;
	uint32_t numBooks;
	uint32_t numActy;
}PUBLIC_USER_INFO;

typedef struct
{
	char name[MAX_BOOK_NAME_LENGTH];
	char desp[MAX_DESP_LENGTH];
	uint32_t tags_bitmap;
	USER_ID owner;
	uint32_t num_WOWs;
	uint32_t num_BUYs;
	uint32_t collected_count;
}PUBLIC_BOOK_INFO;














