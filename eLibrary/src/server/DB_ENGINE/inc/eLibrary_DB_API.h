/*
 * eLibrary_db_API.h
 *
 *  Created on: 5 Feb 2014
 *      Author: murphy
 */

#ifndef ELIBRARY_DB_API_H_
#define ELIBRARY_DB_API_H_

typedef SYSTEM_STATUS (*findBooksByFilter_func)(COMMON_HANDLE host_id, uint32_t filter_id, uint32_t *num_books, COMMON_HANDLE *book_ids);
typedef SYSTEM_STATUS (*sortBooks_func)(uint32_t num_books, COMMON_HANDLE *book_ids);
typedef SYSTEM_STATUS (*findUsersByFilter_func)(COMMON_HANDLE host_id, uint32_t filter_id, uint32_t *num_users, COMMON_HANDLE *user_ids);
typedef SYSTEM_STATUS (*sortUsers_func)(uint32_t num_books, COMMON_HANDLE *user_ids);
typedef SYSTEM_STATUS (*createAccount_func)(char *user_name, COMMON_HANDLE *host_id);
typedef SYSTEM_STATUS (*getUserPublicInfo_func)(COMMON_HANDLE user_id, PUBLIC_USER_INFO *user_info);
typedef SYSTEM_STATUS (*getBookPublicInfo_func)(COMMON_HANDLE book_id, PUBLIC_BOOK_INFO *book_info);

typedef struct
{
	createAccount_func createAccount;
	getUserPublicInfo_func getUserPublicInfo;
	sortUsers_func sortUsers;
	findUsersByFilter_func findUsersByFilter;
	getBookPublicInfo_func getBookPublicInfo;
	sortBooks_func sortBooks;
	findBooksByFilter_func findBooksByFilter;
}DB_ENGINE;

extern DB_ENGINE g_DB_interfaces;

SYSTEM_STATUS eLibrary_DB_init();
SYSTEM_STATUS eLibrary_DB_term();
SYSTEM_STATUS eLibrary_DB_createAccount(char *user_name, COMMON_HANDLE *host_id);
SYSTEM_STATUS eLibrary_DB_getUserPublicInfo(COMMON_HANDLE user_id, PUBLIC_USER_INFO *public_info);
SYSTEM_STATUS eLibrary_DB_sortUsers(uint32_t sort_id, uint32_t num_books, COMMON_HANDLE *user_list);
SYSTEM_STATUS eLibrary_DB_findUsersByFilter(COMMON_HANDLE user_id, PUBLIC_USER_INFO *user_info);
SYSTEM_STATUS eLibrary_DB_getBookPublicInfo(COMMON_HANDLE book_id, PUBLIC_BOOK_INFO *book_info);
SYSTEM_STATUS eLibrary_DB_sortBooks(uint32_t sort_id, uint32_t num_books, COMMON_HANDLE *book_list);
SYSTEM_STATUS eLibrary_DB_findBooksByFilter(COMMON_HANDLE host_id, uint32_t filter_id, uint32_t *num_books, COMMON_HANDLE *book_ids);

#endif /* ELIBRARY_DB_API_H_ */
