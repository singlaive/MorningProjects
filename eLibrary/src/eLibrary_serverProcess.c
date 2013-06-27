/*
 * eLibrary_serverHandler.c
 *
 *  Created on: 2 Nov 2012
 *      Author: singlaive
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include "common.h"
#include "eLibrary_serverHandler.h"
#include "eLibrary_serverProcess.h"

pthread_cond_t cond_exchange = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_exchange = PTHREAD_MUTEX_INITIALIZER;

static struct userList * detechFrdNode(USER_INFO * host, char * frdName);
static struct bookList * detechBookNode(USER_INFO * host, char * bookName);
static struct msgList * getMsgNode(USER_INFO * host, char * frdName);
static BOOK_INFO * getBookByName(USER_INFO * host, char * bookName);
static void sortByDate(struct bookList	* books, uint32_t * numBooks, struct bookList ** bookList);
static void sortByAlfa(struct bookList	* books, uint32_t * numBooks, struct bookList ** bookList);
static void sortByGroup(struct bookList * books, uint32_t * numBooks, struct bookList ** bookList);

static void bookUpLoad(char * bookname);
static void dumpBookInfo(BOOK_INFO * book);

USER_INFO * getUserInfo(char * username, char * email)
{
	USER_INFO * user_found = NULL;
	struct userList * current = g_userData.listHead;
	bool found = false;

	DIAG_INFO("Look for user %s with email %s.", username, email);
uint32_t i= 0U;
	if (0 != pthread_mutex_lock(&g_userData.lock))
	{
		DIAG_ERROR("pthread_mutex_lock failed.");
	}
	else
	{
		while ((false == found) && (NULL != current))
		{
			printf("user[%d]: %s\n", i++, current->user->username);
			if (0 == strncmp(current->user->username, username, strlen(username)))
			{
				if (NULL != email)
				{
					if (0 != strncmp(current->user->email, email, strlen(email)))
					{
						continue;
					}
				}

				found = true;
				user_found = current->user;
				DIAG_INFO("User %s found.", username);
			}

			current = current->next;
		}

		(void)pthread_mutex_unlock(&g_userData.lock);
	}

	if (false == found)
	{
		DIAG_WARN("User %s does not exist.", username);
	}

	return user_found;
}

USER_INFO * getFrdInfo(USER_INFO * host, char * frdName)
{
	USER_INFO * user_found = NULL;
	struct userList * current = host->friends;
	bool found = false;

	while ((false == found) && (NULL != current))
	{
		if (0 == strncmp(current->user->username, frdName, strlen(frdName)))
		{
			found = true;
			user_found = current->user;
		}

		current = current->next;
	}

	return user_found;
}

/*
 *
 * */
SYSTEM_STATUS getBookInfo(BOOK_FILTER * filter, SORT_METHOD sort, uint32_t * numBooks, struct bookList ** books)
{
	SYSTEM_STATUS result = STATUS_OK;

	/* Only a single book shall be returned, if found. The sort method shall be ignored. */
	if (FILTER_BY_BOOK_NAME == filter->tag)
	{
		BOOK_INFO * bookFound = NULL;
		DIAG_INFO("Filter: FILTER_BY_BOOK_NAME.");

		/* Look for a book globally. */
		bookFound = getBookByName(NULL, filter->filter.bookName);
		if (NULL == bookFound)
		{
			/* No book found. */
			*numBooks = 0U;
		}
		else
		{
			*numBooks = 1U;
			*books = malloc(sizeof(struct bookList));
			if (NULL != *books)
			{
				memset(*books, 0x00, sizeof(struct bookList));
				(*books)->book = bookFound;
				(*books)->next = NULL;
			}
			else
			{
				DIAG_ERROR("Failed to malloc memory.");
				result = STATUS_FAIL;
			}
		}
	}
	/* Multiple books can be found, sort method shall apply. */
	else if (FILTER_BY_USER == filter->tag)
	{
		struct bookList * bookList = NULL;
		DIAG_INFO("Filter: FILTER_BY_USER.");

		DIAG_INFO("There are %d books in user %s.", filter->filter.user->numBooks, filter->filter.user->username);

		*numBooks = filter->filter.user->numBooks;
		*books = malloc(*numBooks*sizeof(struct bookList));
		if (NULL != *books)
		{
			memset(*books, 0x00, *numBooks*sizeof(struct bookList));
			switch (sort)
			{
				case SORT_BY_DATE:
					sortByDate(filter->filter.user->books, numBooks, books);
					break;
				case SORT_BY_ALFA:
					sortByAlfa(filter->filter.user->books, numBooks, books);
					break;
				case SORT_BY_GROUP:
					sortByGroup(filter->filter.user->books, numBooks, books);
					break;
				default:
					DIAG_ERROR("Unsupported sort type.");
					result = STATUS_FAIL;
					break;
			}
		}
		else
		{
			DIAG_ERROR("Failed to malloc memory.");
			result = STATUS_FAIL;
		}
	}
	else
	{
		DIAG_ERROR("Unsupported filter type %d.", filter->tag);
		result = STATUS_FAIL;
	}

	DIAG_INFO("Found %d books (address %p).", *numBooks, *books);

	return result;
}

BOOK_INFO * getBookByName(USER_INFO * host, char * bookName)
{
	struct bookList * current = NULL;
	BOOK_INFO * book_found = NULL;
	bool found = false;

	if (NULL == host)
	{
		current = g_bookTotal.head;
		DIAG_INFO("Query global database.");
	}
	else
	{
		current = host->books;
		DIAG_INFO("Query local database of user %s.", host->username);
	}

	while ((false == found) && (NULL != current))
	{
		printf("Library book  %s :: look for %s\n", current->book->name, bookName);
		if ((NULL != host) && (0 != strncmp(current->book->owner->username, host->username, strlen(host->username))))
		{
			break;
		}

		if (0 == strncmp(current->book->name, bookName, strlen(bookName)))
		{
			found = true;
			book_found = current->book;
		}

		current = current->next;
	}

	DIAG_INFO("Found book (%#x)", (uint32_t)book_found);
	return book_found;
}

SYSTEM_STATUS addUser(char * userName, char * email)
{
	SYSTEM_STATUS result = STATUS_FAIL;
	USER_INFO * newUser = NULL;
	struct userList * usernode = NULL;

	if ((NULL == userName) || (NULL == email))
	{
		DIAG_ERROR("Invalid user name.");
	}
	else if (NULL != getUserInfo(userName, email))
	{
		DIAG_WARN("User (name %s) already exists!", userName);
	}
	else
	{
		newUser = malloc(sizeof(USER_INFO));
		if (NULL == newUser)
		{
			DIAG_ERROR("Failed to malloc memory.");
		}
		else
		{
			memset(newUser, 0x00, sizeof(USER_INFO));
			strncpy(newUser->email, email, MAX_EMAIL_LENGTH);
			strncpy(newUser->username, userName, MAX_NAME_LENGTH);
			newUser->activity = NULL;
			newUser->books = NULL;
			newUser->numBooks = 0U;
			newUser->numFrds = 0U;
			newUser->msgBox.numMsg = 0U;
			newUser->msgBox.numUnread = 0U;
			newUser->msgBox.msgs = NULL;

			usernode = malloc(sizeof(struct userList));
			if (NULL != usernode)
			{
				memset(usernode, 0x00, sizeof(struct userList));
				usernode->next = NULL;
				usernode->user = newUser;

				if (0 != pthread_mutex_lock(&g_userData.lock))
				{
					DIAG_ERROR("pthread_mutex_lock() failed.");
				}
				else
				{
					if (0U == g_userData.numUsers)
					{
						g_userData.listHead = usernode;
						g_userData.listTail = usernode;
					}
					else
					{
						g_userData.listTail->next = usernode;
						g_userData.listTail = usernode;
					}

					g_userData.numUsers++;
					(void)pthread_mutex_unlock(&g_userData.lock);

					if (STATUS_OK != addUserActy(newUser, ACTY_FILTER_USER_CREATE, NULL))
					{
						DIAG_ERROR("addUserActy() failed.");
					}
					else
					{
						result = STATUS_OK;
					}

					DIAG_INFO("New user added: name %s; email %s.", userName, email);

				}
			}
			else
			{
				DIAG_ERROR("Failed to malloc memory.");
			}
		}
	}

	return result;
}

SYSTEM_STATUS addFriend(USER_INFO * host, USER_INFO * friend)
{
	SYSTEM_STATUS status = STATUS_FAIL;
	struct userList * current = host->friends;
	bool found = false;
	struct userList * newNode = NULL;

	DIAG_INFO("%s is adding a friend %s.", host->username, friend->username);

	newNode = malloc(sizeof(struct userList));
	if (NULL == newNode)
	{
		DIAG_ERROR("Failed to malloc memory.");
	}
	else
	{
		memset(newNode, 0x00, sizeof(struct userList));
		newNode->user = friend;
		newNode->next = NULL;

		if (0 != host->numFrds)
		{
			DIAG_INFO("Check if this friend has ever been added.");

			for (;;)
			{
				if (0 == strcmp(current->user->username, friend->username))
				{
					found = true;
					DIAG_WARN("This person is already been added.");
					status = STATUS_OK;
					break;
				}

				/* Check if we are the end of list. */
				if (NULL == current->next)
				{
					break;
				}
				else
				{
					current = current->next;
				}
			}

			if (false == found)
			{
				DIAG_INFO("No found, adding new friend.");
				current->next = newNode;
				host->numFrds++;
				addUserActy(host, ACTY_FILTER_ADDING_FRD, friend->username);
				status = STATUS_OK;
			}
		}
		else
		{
			DIAG_INFO("Adding the first friend.");
			host->friends = newNode;
			host->numFrds++;
			status = STATUS_OK;
		}
	}

	return status;
}

SYSTEM_STATUS rmFriend(USER_INFO * host, char * frdName)
{
	SYSTEM_STATUS status = STATUS_FAIL;
	struct userList * frdFound = NULL;

	if (NULL == getFrdInfo(host, frdName))
	{
		DIAG_ERROR("Did not find such friend (%s)", frdName);
	}
	else
	{
		struct userList * nodeToRmed = NULL;

		nodeToRmed = detechFrdNode(host, frdName);
		COMMON_free(nodeToRmed);
		host->numFrds--;
		DIAG_INFO("%s removed from the friend list of %s.", frdName, host->username);

		status = STATUS_OK;
	}

	return status;
}

char * viewBook(char * bookName)
{
	char * buf = NULL;
	buf = handlerView("Char", (cmd_line_tag *)123);

	bookName = bookName;
	return "I know I am reading this book!\n";
}

struct userList * detechFrdNode(USER_INFO * host, char * frdName)
{
	struct userList * nodeToRm = NULL;
	struct userList * current = host->friends;
	struct userList * predecessor = NULL;

	if (NULL == current)
	{
		DIAG_WARN("No friends.");
	}

	while(NULL != current)
	{
		if (0 == strncmp(current->user->username, frdName, strlen(frdName)))
		{
			DIAG_INFO("Got friend %s", frdName);

			if (NULL == predecessor)
			{
				DIAG_INFO("%s is the only one in list.", frdName);

				host->friends = NULL;
			}
			else
			{
				predecessor->next = current->next;
			}

			nodeToRm = current;
			current->next = NULL;
			break;
		}

		predecessor = current;
		current = current->next;
	}

	return nodeToRm;
}

SYSTEM_STATUS addBook(USER_INFO * owner, char * bookName, char * bookDesp, BOOK_GROUP bookGroup)
{
	SYSTEM_STATUS result = STATUS_FAIL;
	BOOK_INFO * bookAdded = NULL;

	if ((NULL == owner) || (NULL == bookName))
	{
		DIAG_ERROR();
	}
	else
	{
		if (NULL == (bookAdded = getBookByName(NULL, bookName)))
		{
			struct bookList * newNodePrivate = NULL;
			struct bookList * newNodePublic = NULL;

			bookUpLoad(bookName);
			bookAdded = malloc(sizeof(BOOK_INFO));
			if (NULL == bookAdded)
			{
				DIAG_ERROR("Failed to malloc memory.");
			}
			else
			{
				memset(bookAdded, 0x00, sizeof(BOOK_INFO));
				strncpy(bookAdded->name, bookName, strlen(bookName));
				bookAdded->owner = owner;
				bookAdded->numBuy = 0U;
				bookAdded->numWow = 0U;
				bookAdded->whoBuyed = NULL;
				bookAdded->whoWowed = NULL;
				addUserActy(owner, ACTY_FILTER_ADDING_BOOK, bookName);

				newNodePrivate = malloc(sizeof(struct bookList));
				if (NULL == newNodePrivate)
				{
					DIAG_ERROR("Failed to malloc memory.");
				}
				else
				{
					memset(newNodePrivate, 0x00, sizeof(struct bookList));
					newNodePrivate->book = bookAdded;
					newNodePrivate->next = NULL;

					if (0U == owner->numBooks)
					{
						owner->books = newNodePrivate;
					}
					else
					{
						owner->books->next = newNodePrivate;
					}

					owner->numBooks++;
					DIAG_INFO("Book %s added to local database; numBooks = %d.", bookAdded->name, owner->numBooks);
				}

				newNodePublic = malloc(sizeof(struct bookList));
				if (NULL == newNodePublic)
				{
					DIAG_ERROR("Failed to malloc memory.");
				}
				else
				{
					memset(newNodePublic, 0x00, sizeof(struct bookList));
					newNodePublic->book = bookAdded;
					newNodePublic->next = NULL;

					if (0U == g_bookTotal.numBooks)
					{
						g_bookTotal.head = newNodePublic;
						g_bookTotal.tail = newNodePublic;
					}
					else
					{
						g_bookTotal.tail->next = newNodePublic;
						g_bookTotal.tail = newNodePublic;
					}

					g_bookTotal.numBooks++;
					DIAG_INFO("Book %s added to global database; numBooks = %d.", bookAdded->name, g_bookTotal.numBooks);
				}
			}
		}

		if (book_group_undef != bookGroup)
			bookAdded->group = bookGroup;
		if (NULL != bookDesp)
			strncmp(bookAdded->desp, bookDesp, strlen(bookDesp));

		dumpBookInfo(bookAdded);
		result = STATUS_OK;
	}

	return result;
}

SYSTEM_STATUS rmBook(USER_INFO * host, char * bookName)
{
	SYSTEM_STATUS result = STATUS_FAIL;
	BOOK_INFO * bookFound = NULL;

	if ((NULL == host) || (NULL == bookName))
	{
		DIAG_ERROR();
	}
	else
	{
		if (NULL != (bookFound = getBookByName(host, bookName)))
		{
			struct bookList * nodeToRmed = NULL;

			if (--bookFound->countCollected == 0)
			{
				nodeToRmed = detechBookNode(NULL, bookName);
				COMMON_free(bookFound);
				COMMON_free(nodeToRmed);
				g_bookTotal.numBooks--;
				DIAG_INFO("Book \"%s\" removed from eLibrary.", bookName);
			}
			else
			{
				bookFound->owner = NULL;
			}

			nodeToRmed = detechBookNode(host, bookName);
			COMMON_free(nodeToRmed);
			host->numBooks--;
			DIAG_INFO("Book \"%s\" removed from %s.", bookName, host->username);
		}
		else
		{
			DIAG_WARN("The host does not own such book.");
		}

		result = STATUS_OK;
	}

	return result;
}

struct bookList * detechBookNode(USER_INFO * host, char * bookName)
{
	struct bookList * nodeToRm = NULL;
	struct bookList * current = NULL;
	struct bookList * predecessor = NULL;

	current = (NULL == host) ? g_bookTotal.head : host->books;

	if (NULL == current)
	{
		DIAG_WARN("No books.");
	}

	while(NULL != current)
	{
		if (0 == strncmp(current->book->name, bookName, strlen(bookName)))
		{
			DIAG_INFO("Got book \"%s\"", bookName);

			if (NULL == predecessor)
			{
				DIAG_INFO("Book \"%s\" is the only one in list.", bookName);

				if (NULL == host)
				{
					g_bookTotal.head = NULL;
				}
				else
				{
					host->books = NULL;
				}
			}
			else
			{
				predecessor->next = current->next;
			}

			nodeToRm = current;
			current->next = NULL;
			break;
		}

		predecessor = current;
		current = current->next;
	}

	return nodeToRm;
}

/* name can be friend name or book name */
SYSTEM_STATUS addUserActy(USER_INFO * host, ACTY_FILTER_TAG filter, char * name)
{
	char buf[MAX_ACTY_LENGTH];
	struct actyList * acty = NULL;
	time_t timestamp = time(NULL);

	DIAG_INFO("addUserActy() starts.");
	DIAG_INFO("Filter type: %d.", filter);

	switch (filter)
	{
		case ACTY_FILTER_USER_CREATE:
			snprintf(buf, MAX_ACTY_LENGTH, "%s %s joined eLibrary!", myAscTimeLite(&timestamp), host->username);
			break;
		case ACTY_FILTER_ADDING_BOOK:
			snprintf(buf, MAX_ACTY_LENGTH, "%s %s added \"%s\"!", myAscTimeLite(&timestamp), host->username, name);
			break;
		case ACTY_FILTER_ADDING_FRD:
			snprintf(buf, MAX_ACTY_LENGTH, "%s %s and %s are friends now!", myAscTimeLite(&timestamp), host->username, name);
			break;
		case ACTY_FILTER_DELETING_BOOK:
			snprintf(buf, MAX_ACTY_LENGTH, "%s %s deleted \"%s\".", myAscTimeLite(&timestamp), host->username, name);
			break;
		case ACTY_FILTER_BY_WOW:
			snprintf(buf, MAX_ACTY_LENGTH, "%s %s added a WOW to \"%s\"!", myAscTimeLite(&timestamp), host->username, name);
			break;
		case ACTY_FILTER_BY_BUY:
			snprintf(buf, MAX_ACTY_LENGTH, "%s %s added a BUY to \"%s\"!", myAscTimeLite(&timestamp), host->username, name);
			break;
		default:
			break;
	}

	DIAG_INFO("User history: %s", buf);
	acty = malloc(sizeof(struct actyList));
	if (NULL == acty)
	{
		DIAG_ERROR("Failed to allocate memory.");
		return STATUS_FAIL;
	}
	else
	{
		printf("==========================================================Acty address assgned: %p\n", acty);
	}

	memset(acty, 0x00, sizeof(struct actyList));
	strncpy(acty->acty, buf, strlen(buf));
	acty->timestamp = timestamp;
	acty->next = NULL;

	if (NULL == host->activity)
	{
		host->activity = acty;
		host->lastActy = acty;
	}
	else
	{
		host->lastActy->next = acty;
		host->lastActy = acty;
	}

	host->numActy++;

	strncpy(g_bookTotal.activities[g_bookTotal.actyTailOffset], buf, strlen(buf));

	DIAG_INFO("addUserActy() ends.");
	return STATUS_OK;
}

SYSTEM_STATUS getUserActy(USER_INFO * host, ACTY_FILTER_TAG filter, uint32_t * numActy, struct actyList ** activities)
{
	struct actyList * current = host->activity;
	struct actyList * p = NULL;
	char * str = NULL;
	SYSTEM_STATUS result = STATUS_OK;

	DIAG_INFO("getUserActy(): starts");
	DIAG_INFO("Get activities from %s with filter %d.", host->username, filter);

	if ((NULL == host) || (NULL == numActy) || (NULL == activities))
	{
		DIAG_ERROR("Invalid parameter: host=%p; numActy=%p; activities=%p.", host, numActy, activities);
		result = STATUS_FAIL;
	}
	else if (ACTY_FILTER_ALL == filter)
	{
		DIAG_INFO("Filter = ACTY_FILTER_ALL (%d).", ACTY_FILTER_ALL);

		*numActy = host->numActy;
		*activities = host->activity;
	}
	else if (ACTY_FILTER_BY_WOW == filter)
	{
		DIAG_INFO("Filter = ACTY_FILTER_BY_WOW (%d).", ACTY_FILTER_BY_WOW);
		str = "WOW";

		result = STATUS_OK;
	}
	else if (ACTY_FILTER_BY_BUY == filter)
	{
		DIAG_INFO("Filter = ACTY_FILTER_BY_BUY (%d).", ACTY_FILTER_BY_BUY);

		str = "BUY";
		result = STATUS_OK;
	}
	else
	{
		DIAG_ERROR("Invalid filter type (%d).", filter);
	}

	if ((STATUS_OK == result) && (NULL != str))
	{
		uint32_t counter = 0U;
uint32_t i = 0U;
printf("numActy = %d\n", host->numActy);

		while (NULL != current)
		{
			printf("Full acty address: %p\n", current->acty);
			if (NULL != strstr(current->acty, str))
			{
				struct actyList * acty = malloc(sizeof(struct actyList));
				if (NULL == acty)
				{
					DIAG_ERROR("Failed to malloc memory.");
					result = STATUS_FAIL;
					break;
				}
				else
				{
					memcpy(acty, current, sizeof(struct actyList));
					acty->next = NULL;

					if (NULL == *activities)
					{
						*activities = acty;
						p = acty;
					}
					else
					{
						p->next = acty;
						p = p->next;

					}
						printf("First: p = %p\n", p);

					DIAG_INFO("Copied activity: \"%s\".", acty->acty);
					counter++;
				}
			}
			else
			{
						printf("Not my want: %s (%p, %p)\n", current->acty, current, current->acty);
			}

			current = current->next;
		}

		*numActy = counter;
		DIAG_INFO("%d activities copied.", *numActy);
	}

	DIAG_INFO("getUserActy(): ends.");
	return result;
}

SYSTEM_STATUS getBookHty(BOOK_INFO * book, ACTY_FILTER_TAG filter, uint32_t * counter, struct actyList ** activities)
{
	SYSTEM_STATUS status = STATUS_OK;
	uint32_t amountExpected = 0U;
	struct nameDateList * current = NULL;
	char buf[MAX_ACTY_LENGTH];
	char * act;

	if (ACTY_FILTER_BY_WOW == filter)
	{
		current = book->whoWowed;
		amountExpected = book->numWow;
		act = "WOW";
	}
	else if (ACTY_FILTER_BY_BUY == filter)
	{
		current = book->whoBuyed;
		amountExpected = book->numBuy;
		act = "BUY";
	}
	else
	{
		DIAG_ERROR("Unsupported parameters (filter = %#x).", filter);
		status = STATUS_INVALID_PARAM;
	}

	if (STATUS_OK == status)
	{
		*counter = 0U;
		struct actyList * ptr = NULL;

		while (NULL != current)
		{
			struct actyList * acty = NULL;

			snprintf(buf, MAX_ACTY_LENGTH, "%s \"%s\" got a %s by %s!", myAscTimeLite(&current->date), book->name, act, current->name);
			DIAG_INFO("Book history: %s", buf);
			acty = malloc(sizeof(struct actyList));
			if (NULL == acty)
			{
				DIAG_ERROR("Failed to allocate memory.");
				status = STATUS_FAIL;
				actyFree(filter, ptr, *counter);
				break;
			}

			memset(acty, 0x00, sizeof(struct actyList));
			(*counter)++;
			strncpy(acty->acty, buf, strlen(buf));
			acty->next = NULL;

			if (NULL != ptr)
				ptr->next = acty;
			ptr = acty;
			current = current->next;
		}

		if (*counter == amountExpected)
		{
			*activities = 0; /***********************************************************/
			status = STATUS_OK;
		}
		else
		{
			DIAG_ERROR("The amount of wowed (%d, expected %d) is incorrect.", *counter, amountExpected);
			status = STATUS_FAIL;
			*counter = 0U;
		}
	}

	return status;
}

SYSTEM_STATUS mergeActy(USER_INFO * host, struct actyList * activities, uint32_t * counter)
{
	SYSTEM_STATUS result = STATUS_OK;
	uint32_t count = 0U;
	uint32_t i, j;
	struct actyList * acties_temp = NULL;

	for (i = 0U; i < host->numFrds; i++)
	{
		struct actyList * current = host->friends[i].user->activity;

		for (j = 0U; j < host->friends[i].user->numActy; j++)
		{
			struct actyList * ptr = acties_temp;
			struct actyList * prv = ptr;

			/* Add a check voiding some friend with long activities but long time no update. */
			do
			{
				if ((0U == count) || ((current->timestamp > ptr->timestamp) && (count < MAX_ACTIES_AMOUNT)))
				{
						struct actyList * newnode = malloc(sizeof(struct actyList));
						if (NULL == newnode)
						{
							DIAG_ERROR("Failed to malloc memory.");
							result = STATUS_FAIL;
							break;
						}
						else
						{
							memcpy(newnode, current, sizeof(struct actyList));
							newnode->next = (NULL == ptr) ? NULL : ptr->next;

							if (NULL != prv) prv->next = newnode;
							/* Either acties_temp and prv both are NULL, meaning there is no node at first;
							 * or we just added a new node to the head of the list: we all need to update acties_temp. */
							if (prv == acties_temp) acties_temp = newnode;

							count++;
						}
				}

				prv = ptr;
				ptr = ptr->next;
			}while (NULL != ptr);

			current = current->next;
		}
	}

	return result;
}

SYSTEM_STATUS wowBook(USER_INFO * host, BOOK_INFO * book)
{
	SYSTEM_STATUS result = STATUS_FAIL;

	DIAG_INFO("%s WOWs %s.", host->username, book->name);

	/* Check if trying to WOW the own's own book. */
	if (0 == strncmp(book->owner->username, host->username, strlen(host->username)))
	{
		DIAG_INFO("Can not WOW one's own book( book owner %s; host %s).", book->owner->username, host->username);
		return STATUS_FAIL;
	}
	else
	{
		book->numWow++;
		struct nameDateList * wowRecord = malloc(sizeof(struct nameDateList));
		if (NULL == wowRecord)
		{
			DIAG_ERROR("Failed to allocate memory.");
			result = STATUS_FAIL;
		}
		else
		{
			memset(wowRecord, 0x00, sizeof(struct nameDateList));
			wowRecord->date;/*-------------------------What are you doing here? ----------------------------*/
			strncpy(wowRecord->name, host->username, strlen(host->username));
			wowRecord->next = book->whoWowed;
			book->whoWowed = wowRecord;

			result = addUserActy(host, ACTY_FILTER_BY_WOW, book->name);
			if (STATUS_OK != result)
			{
				DIAG_ERROR("addUserActy() failed when WoWing book %s by %s.", book->name, host->username);
			}
		}
	}

	return result;
}

SYSTEM_STATUS buyBook(USER_INFO * host, BOOK_INFO * book)
{
	SYSTEM_STATUS result = STATUS_FAIL;


	/* Check if trying to BUY the own's own book. */
	if (0 == strncmp(book->owner->username, host->username, strlen(host->username)))
	{
		return STATUS_FAIL;
	}
	else
	{
		book->numBuy++;
		struct nameDateList * buyRecord = malloc(sizeof(struct nameDateList));
		if (NULL == buyRecord)
		{
			DIAG_ERROR("Failed to allocate memory.");
			result = STATUS_FAIL;
		}
		else
		{
			memset(buyRecord, 0x00, sizeof(struct nameDateList));
			buyRecord->date;/*-------------------------What are you doing here? ----------------------------*/
			strncpy(buyRecord->name, host->username, strlen(host->username));
			buyRecord->next = book->whoBuyed;
			book->whoBuyed = buyRecord;

			result = addUserActy(host, ACTY_FILTER_BY_BUY, book->name);
			if (STATUS_OK != result)
			{
				DIAG_ERROR("addUserActy() failed when WoWing book %s by %s.", book->name, host->username);
			}
		}
	}

	return result;
}

SYSTEM_STATUS getMsgInfo(USER_INFO * host, char * frdName, struct msgList ** msgNode)
{
	SYSTEM_STATUS result = STATUS_OK;

	DIAG_INFO("getMsgInfo(): starts.");
	if ((NULL == host) || (NULL == frdName) || (NULL == msgNode))
	{
		DIAG_ERROR("Invalid parameters.");
		result = STATUS_INVALID_PARAM;
	}
	else
	{
		DIAG_INFO("Host: %s.", host->username);
		DIAG_INFO("Context: %s", frdName);
		*msgNode = getMsgNode(host, frdName);
		if (NULL == msgNode)
		{
			DIAG_ERROR("Did not find the context.");
		}
	}

	DIAG_INFO("getMsgInfo(): end (%d).", result);
	return result;
}

SYSTEM_STATUS notifyExchange(USER_INFO * host, char * frdName, char * msg)
{
	SYSTEM_STATUS result = STATUS_OK;
	char systemMsg[MAX_EVENT_LENGTH];
	bool msgSent = false;

	DIAG_INFO("notifyExchange(): start.");

	memset(systemMsg, 0x00, MAX_EVENT_LENGTH);
	memcpy(systemMsg, host->username, (size_t)MAX_NAME_LENGTH);
	memcpy(systemMsg+MAX_NAME_LENGTH, frdName, (size_t)MAX_NAME_LENGTH);
	memcpy(systemMsg+2*MAX_NAME_LENGTH, msg, MAX_MSG_LENGTH);

	while ((false == msgSent) && (STATUS_OK == result))
	{
		DIAG_INFO("g_systemEvent = %d", g_systemEvent);

		if (0 == pthread_mutex_lock(&lock_exchange))
		{
			if (sys_evt_undef == g_systemEvent)
			{
				g_systemEvent = SYSTEM_EVENT_MSG_SENT;
				memcpy(g_sysEvtPayload, systemMsg, MAX_EVENT_LENGTH);

				if (0 != pthread_cond_signal(&cond_exchange))
				{
					DIAG_ERROR("pthread_cond_signal failed.");
					result = STATUS_FAIL;
				}
				else
				{
					DIAG_INFO("System message delivered!");
					msgSent = true;
				}
			}
			else
			{
				DIAG_INFO("Last ystem message not handled yet.");
				if (0 != pthread_mutex_unlock(&lock_exchange))
				{
					DIAG_ERROR("pthread_mutex_unlock failed.");
					result = STATUS_FAIL;
				}
				else
				{
					DIAG_INFO("After 1sec, retry deliverying the system message!.");
					sleep(1);
				}
			}
		}
		else
		{
			DIAG_ERROR("pthread_mutex_lock failed.");
			result = STATUS_FAIL;
		}
	}

	if ((true == msgSent) && (STATUS_OK == result))
	{
		if (0 != pthread_mutex_unlock(&lock_exchange))
		{
			DIAG_ERROR("pthread_mutex_unlock failed.");
			result = STATUS_FAIL;
		}
	}

	DIAG_INFO("notifyExchange(): end.");
	return result;
}
SYSTEM_STATUS sendReceiveMsg(USER_INFO * host, char * frdName, char * msg, MSG_DIRECTION direction)
{
	struct msgContext * newContext = NULL;
	struct msgList * msgNode = NULL;
	SYSTEM_STATUS result = STATUS_OK;

	DIAG_INFO("sendReceiveMsg(): from %s; to %s; msg \"%s\"", host->username, frdName, msg);

	if ((NULL == msg) || (NULL == host) || (NULL == frdName))
	{
		DIAG_ERROR("Invalid parameter.");
		result = STATUS_INVALID_PARAM;
	}
	else if (false == g_msgExchange_ready)
	{
		DIAG_ERROR("Message exchange thread not ready yet !");
		result = STATUS_FAIL;
	}
	else
	{
		newContext = malloc(sizeof(struct msgContext));
		if (NULL == newContext)
		{
			DIAG_ERROR("Failed to alloc memory.");
			result = STATUS_FAIL;
		}
		else
		{
			memset(newContext, 0x00, sizeof(struct msgContext));
			newContext->msg.io = direction;
			if (direction == MSG_RECEIPT_IN)
			{
				newContext->msg.unread = true;
			}
			else if (direction == MSG_RECEIPT_OUT)
			{
				newContext->msg.unread = false;
			}
			else
			{}

			newContext->msg.datetime = time(NULL);
			strncpy(newContext->msg.msg, msg, MAX_MSG_LENGTH);
			msgNode = getMsgNode(host, frdName);
			if (NULL == msgNode)
			{
				DIAG_INFO("First message to %s.", frdName);

				msgNode = malloc(sizeof(struct msgList));
				if (NULL == msgNode)
				{
					DIAG_ERROR("Failed to allocate memory.");
					result = STATUS_FAIL;
				}
				else
				{
					memset(msgNode, 0x00, sizeof(struct msgList));
					strncpy(msgNode->contact, frdName, strlen(frdName));
					msgNode->next = host->msgBox.msgs;
					host->msgBox.msgs = msgNode;
				}
			}
			else
			{
				DIAG_INFO("Existing contact %s", frdName);
			}
		}

		if (STATUS_OK == result)
		{
			newContext->next = msgNode->contexts;
			msgNode->contexts = newContext;

			msgNode->numMsg++;
			host->msgBox.numMsg++;

			if (direction == MSG_RECEIPT_IN)
			{
				msgNode->numUnread++;
				host->msgBox.numUnread++;
				DIAG_INFO("Message received.");
			}
			else if (direction == MSG_RECEIPT_OUT)
			{
				result = notifyExchange(host, frdName, msg);
				if (STATUS_OK != result)
				{
					DIAG_ERROR("nofifyExchange() failed.");
				}
				else
				{
					DIAG_INFO("Message sent.");
				}
			}
			else
			{}
		}
		else if (NULL != newContext)
		{
			COMMON_free(newContext);
		}
		else
		{}
	}

	DIAG_INFO("sendReceiveMsg(): ends (%d).", result);
	return result;
}

struct msgList * getMsgNode(USER_INFO * host, char * frdName)
{
	bool found = false;
	struct msgList * current = host->msgBox.msgs;

	while (NULL != current)
	{
	DIAG_INFO("Looking for %s; current %s", frdName, current->contact);
		if (0 == strcmp(frdName, current->contact))
		{
			found = true;
			break;
		}

		current = current->next;
	}
printf("found node %d\n", found);
	return ((true == found)?current:NULL);
}

void sortByDate(struct bookList	* books, uint32_t * numBooks, struct bookList ** bookList)
{
}

void sortByAlfa(struct bookList	* books, uint32_t * numBooks, struct bookList ** bookList)
{}

void sortByGroup(struct bookList * books, uint32_t * numBooks, struct bookList ** bookList)
{}

void bookUpLoad(char * bookname)
{}

void dumpBookInfo(BOOK_INFO * book)
{}

BOOK_GROUP groupConverer(char * group_str)
{

}
































