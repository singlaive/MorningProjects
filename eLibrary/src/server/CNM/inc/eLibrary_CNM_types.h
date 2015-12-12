/*
 * eLibrary_CNM_types.h
 *
 *  Created on: 8 Feb 2014
 *      Author: murphy
 */

#ifndef ELIBRARY_CNM_TYPES_H_
#define ELIBRARY_CNM_TYPES_H_

typedef struct
{
	int fifo_fd;
	int socket_fd;
	struct pollfd fd_table;
	nfds_t num_fds;
	void event_CB;
	LIST_HANDLE conn_list;
}CNM_CONTEXT;


typedef enum
{
	CNM_CONN_TYPE_invalid = -1,
	CNM_CONN_TYPE_LOCAL,
	CNM_CONN_TYPE_REMOTE,
	CNM_CONN_TYPE_lastvalue
}CNM_CONN_TYPE;

#endif /* ELIBRARY_CNM_TYPES_H_ */
