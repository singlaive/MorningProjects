/*
 * eLibrary_server_db.c
 *
 *  Created on: 4 Sep 2013
 *      Author: singlaive
 */

#include "eLibrary_sharedTypes.h"
#include "eLibrary_serverTypes.h"
#include "eLibrary_DB_API.h"
#include "tinySQL_API.h"
#include "tinySQL_types.h"

BOOK_LIBRARY g_server_library;

DB_ENGINE g_DB_interfaces =
{
	eLibrary_DB_createAccount,
	eLibrary_DB_setPassword,
	eLibrary_DB_getUserInfo,
	eLibrary_DB_cacheUser,
	eLibrary_DB_exportUserInfo,
	eLibrary_DB_sortUsers,
	eLibrary_DB_findUsersByFilter,
	eLibrary_DB_getBookPublicInfo,
	eLibrary_DB_sortBooks,
	eLibrary_DB_findBooksByFilter
};

/* Account creating shall write to DB immediately. */
static SYSTEM_STATUS eLibrary_DB_createAccount(char *user_name, COMMON_HANDLE *user_id)
{
	SYSTEM_STATUS result;
	USER_INFO *user_info = NULL;
	char buf[MAX_LENGTH];
	uint32_t epoch_time;

	DB_create_user(user_info);
	COMMON_HDL_createObject(user_info, &user_id);
	strncpy(user_info->name, user_name, MAX_NAME_LENGTH);

	result = DB_createTEXT(buf, user_name);
	result = tinySQL_INSERT("users", "user_name", buf);

	result = tinySQL_INSERT("users", "date_registered", time(NULL));
	return result;
}

/* Shall write to DB immediately. */
static SYSTEM_STATUS eLibrary_DB_setPassword(COMMON_HANDLE user_id, char *pw_string)
{
	SYSTEM_STATUS result;
	USER_INFO *user_data = NULL;

	result = COMMON_HDL_getObject(user_id, user_data);
	result = tinySQL_UPDATE("elibrary_db", "users", "password", DB_matchUser, user_data->name, pw_string)
	return result;
}

static SYSTEM_STATUS eLibrary_DB_getUserPublicInfo(COMMON_HANDLE user_handle, PUBLIC_USER_INFO *public_info)
{
	SYSTEM_STATUS result;
	USER_INFO *user_info = NULL;

	COMMON_HDL_getObject(user_handle, &user_info);
	public_info->user_id = user_handle;
	strncpy(public_info->user_name, user_info->name, max);
	public_info->numActy = user_info->numActy;
	public_info->numFrds = user_info->numFriends;
	public_info->numBooks = user_info->numBooksTotal;

	return result;
}

static SYSTEM_STATUS eLibrary_DB_cacheUser(char *user_name, COMMON_HANDLE *user_handle)
{
	SYSTEM_STATUS result;
	uint32_t num_match = 0U;
	void *value;
	USER_INFO *user_info = NULL;

	DB_create_user(user_info);
	result = tinySQL_SELECT("users", "num_uploaded", DB_matchUser, user_name, &num_match, (void*)&user_info->numBooksUploaded);
	result = tinySQL_SELECT("users", "uploaded_books_id", DB_matchUser, user_name, &num_match, book_list);
	result = tinySQL_SELECT("users", "num_collected", DB_matchUser, user_name, &num_match, (void*)&user_info->numBooksUploaded);
	result = tinySQL_SELECT("users", "collected_books_id", DB_matchUser, user_name, &num_match, book_list);
	result = tinySQL_SELECT("users", "last_read_book_id", DB_matchUser, user_name, &num_match, (void*)&user_info->book_id_last_viewed);
	result = tinySQL_SELECT("users", "last_read_page", DB_matchUser, user_name, &num_match, (void*)&user_info->page_last_viewed);
	result = tinySQL_SELECT("users", "recently_WOWed_books_id", DB_matchUser, user_name, &num_match, book_list);
	result = tinySQL_SELECT("users", "message_boxes", DB_matchUser, user_name, &num_match, (void*)&user_info->msgBox);
	result = tinySQL_SELECT("users", "recently_BUYed_books_id", DB_matchUser, user_name, &num_match, book_list);
	result = tinySQL_SELECT("users", "last_read_page", DB_matchUser, user_name, &num_match, (void*)&user_info->numFriends);
	result = tinySQL_SELECT("users", "recently_WOWed_books_id", DB_matchUser, user_name, &num_match, name_list);
	result = tinySQL_SELECT("users", "date_registered", DB_matchUser, user_name, &num_match, (void*)&user_info->date_registered);
	result = tinySQL_SELECT("users", "date_last_visited", DB_matchUser, user_name, &num_match, (void*)&user_info->date_last_visted);

	COMMON_HDL_handleCreate(user_info, user_handle);

	return result;
}

/* Why do we need number of books? */
static SYSTEM_STATUS eLibrary_DB_sortUsers(uint32_t sort_id, uint32_t *num_users, COMMON_HANDLE *user_list)
{
	SYSTEM_STATUS result;

	/* Do some sorting here! */
	return result;
}

/* Where is the filter itself??? */
static SYSTEM_STATUS eLibrary_DB_findUsersByFilter(COMMON_HANDLE user_handle,
												   uint32_t filter_id,
												   uint32_t num_users,
												   uint32_t *num_user_ids)
{
	SYSTEM_STATUS result;

	if (FILTER_BY_USER_NAME & filter_id)
	{
		result = eLibrary_DB_findUserCache(user_name, &user_handle);
		if (NOT_FOUND == result)
		{
			result = eLibrary_DB_cacheUser(user_name, &user_handle);
		}
	}
	else
	{
		if (FILTER_BY_USER_SELF & filter_id)
		{
			*num_users = 1U;
			*user_ids = user_id;
		}
		else if (FILTER_BY_USER_FRIENDLY & filter_id)
		{
			COMMON_HDL_getObject(user_handle, user_info);
			*num_users = user_info->numFriends;
			/* Get friends list of the user handle. */
		}
		else
		{
			/* should never be here */
		}
	}
	return result;
}

/* In what scenario we need cache a book and what do we know about the book? */
static SYSTEM_STATUS eLibrary_DB_cacheBook(char *user_name, COMMON_HANDLE *book_handle)
{
	SYSTEM_STATUS result;
	uint32_t num_match = 0U;
	void *value;
	USER_INFO *user_info = NULL;

	DB_create_user(user_info);
	result = tinySQL_SELECT("users", "num_uploaded", DB_matchUser, user_name, &num_match, (void*)&user_info->numBooksUploaded);

	COMMON_HDL_handleCreate(user_info, user_handle);

	return result;
}
static SYSTEM_STATUS eLibrary_DB_getBookPublicInfo(COMMON_HANDLE book_id, PUBLIC_BOOK_INFO *public_info)
{
	SYSTEM_STATUS result;
	BOOK_INFO *book_info = NULL;

	COMMON_HDL_getObject(book_id, &book_info);
	strncpy(public_info->name, book_info->name, max);
	strncpy(public_info->desp, book_info->desp, max);
	public_info->tags_bitmap = book_info->tags_bitmap;
	public_info->owner = book_info->owner;
	public_info->num_WOWs = book_info->num_WOWs;
	public_info->num_BUYs = book_info->num_BUYs;
	public_info->collected_count = book->collected_count;

	return result;
}
static SYSTEM_STATUS eLibrary_DB_sortBooks(uint32_t sort_id, uint32_t num_books, COMMON_HANDLE *book_list)
{
	SYSTEM_STATUS result;

	/* Do some sorting here! */
	return result;
}

static SYSTEM_STATUS eLibrary_DB_findBooksByFilter(COMMON_HANDLE host_id, uint32_t filter_id, uint32_t *num_books, COMMON_HANDLE *book_ids)
{
	SYSTEM_STATUS result;

FILTER_BY_OBJ_MASK;
FILTER_BY_BOOK_NAME;
FILTER_BY_TAG;

FILTER_BY_ATTR_MASK;
FILTER_BY_COLLECTED;
FILTER_BY_UPLOADED;
FILTER_BY_WOWED;
FILTER_BY_BUYED;
	return result;
}

SYSTEM_STATUS eLibrary_DB_init()
{
	SYSTEM_STATUS result;

	char *table_users_columns[NUM_TABLE_USERS_COLUMNS] =
	{
		"user_index"					/* uint32_t */
		"user_name",					/* string */
		"password",						/* string */
		"num_uploaded",					/* uint32_t */
		"uploaded_books_id",			/* string, format: "id:id:id"*/
		"num_collected",				/* uint32_t */
		"collected_books_id",			/* string, format: "id:id:id"*/
		"last_read_book_id",			/* string, format: "id:id:id"*/
		"last_read_page",				/* uint32_t */
		"recently_WOWed_books_id",		/* string, format: "id:id:id"*/
		"recently_BUYed_books_id",		/* string, format: "id:id:id"*/
		"message_boxes"					/* blob */
		"num_friends"					/* uint32_t */
		"friend"						/* string, format: "id:id:id"*/
		"date_registered",				/* date */
		"date_last_visited"				/* date */
	};

	tinySQL_DATA_TYPE table_users_types[NUM_TABLE_USERS_COLUMNS] =
	{
		tinySQL_DATA_TYPE_UINT32,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_UINT32,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_UINT32,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_UINT32,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_BLOB,
		tinySQL_DATA_TYPE_UINT32,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_DATE,
		tinySQL_DATA_TYPE_DATE
	};

	char *table_books_columns[NUM_TABLE_BOOKS_COLUMNS] =
	{
		"book_id",						/* uint32_t */
		"book_name",					/* string */
		"uploader",						/* uint32_t */
		"uploded_date"					/* date*/
		"num_collected",				/* uint32_t */
		"collecting_users_id",			/* string, format: "id:id:id"*/
		"recently_WOWed_users_id",		/* string, format: "id:id:id"*/
		"recently_BUYed_users_id",		/* string, format: "id:id:id"*/
	};

	tinySQL_DATA_TYPE table_books_types[NUM_TABLE_BOOK_COLUMNS] =
	{
		tinySQL_DATA_TYPE_UINT32,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_UINT32,
		tinySQL_DATA_TYPE_DATE,
		tinySQL_DATA_TYPE_UINT32,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_TEXT,
		tinySQL_DATA_TYPE_TEXT
	};

	result = tinySQL_CREATE("elibrary_db",
							"users",
							NUM_TABLE_USERS_COLUMNS,
							table_users_columns,
							&table_users_types);

	result = tinySQL_CREATE("elibrary_db",
							"books",
							NUM_TABLE_BOOKS_COLUMNS,
							table_books_columns,
							&table_books_types);
}


SYSTEM_STATUS eLibrary_DB_term()
{
	/* close database? */
}












