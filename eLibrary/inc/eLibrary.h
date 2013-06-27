/*
 * eLibrary.h
 *
 *  Created on: 11 Oct 2012
 *      Author: singlaive
 */

#ifndef ELIBRARY_H_
#define ELIBRARY_H_

#define LINELEN_MAX 80U
#define MAX_PAYLOAD_SIZE 320U












/* Size of formatted info which will be sent back to client. */
 /* local commands:  view: list all bookes on shelf; list friend
 * 						-b : show a list of all books on shelf, by default it is sorted by date
 * 						-b string: show a given book
 * 						-b -a :show all books in alphabetical order
 * 						-b -g string: show books online for a given genre
 * 						-f show a list of all freiends
 * 						-f string: show a brief report of this friend
 * 						-f -b string: show books belonging to a given friend
 * 						-f -f string: show all friends of a given friend
 * 					add: add one book online, add description, add genre, add a friend, add a book from others
 * 						-b string: add a book by name, search online database first, if not, add from local
 * 						-d string string: add a description to a book
 * 						-g string string: group a book, or add a group if there is only one parameter
 * 						-f string: add a friend
 * 					rm: remove one book offline, or a friend
 * 						-b string: remove a book offline by name (if no one want the book, the owner shall be admin)
 * 						-f string: remove a friend
 * 					search: serch people, or books
 *						-b string: search a book by name
 *						-f string: search a friend by name
 * 					whatsup: see what books friends are reading, what activity they recently did, can specify a friend
 * 						- : list recent activity by friends and the user himself
 * 						- f string: list a friend's recent activity
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
 *
 *
 * */

void userInfoDump(USER_INFO * userInfo);
SYSTEM_STATUS syncServer(USER_INFO * userInfo);


#endif /* ELIBRARY_H_ */
