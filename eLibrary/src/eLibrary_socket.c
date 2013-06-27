/*
 * eLibrary.c

 *
 *  Created on: 8 Oct 2012
 *      Author: singlaive
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <poll.h>
#include "../../common/inc/common.h"
#include "../inc/eLibrary_socketClient.h"

SYSTEM_STATUS client_socketConnInit()
{
	SYSTEM_STATUS result = STATUS_FAIL;
	int socket_fd = -1;
	struct sockaddr_in addr_server;
	uint32_t port_server;
	char * ip_server = "127.0.0.1";

	do
	{
		/* Create a IPV4 socket. */
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd < 0)
		{
			DIAG_FATAL("socket (socket_fd = %d).", socket_fd);
			break;
		}

		/* Assign a random port: 9877 and a local host address. Those can be in a ini file. */
		bzero(&addr_server, sizeof(addr_server));
		addr_server.sin_family = AF_INET;
		addr_server.sin_port = htons(port_server);
		inet_pton(AF_INET, ip_server, &addr_server.sin_addr);

		result = connect(socket_fd, (struct sockaddr *)&addr_server, sizeof(addr_server));
		if (result < 0)
		{
			DIAG_FATAL("Failed to connect to server! (result = %d)", result);
			break;
		}
		else
		{
			DIAG_INFO("TCP connection is established. (result = %d)", result);
		}

		result = STATUS_OK;
	}while (0);

	return result;
}
SYSTEM_STATUS server_msgSend();
SYSTEM_STATUS client_socketDataUpdate(void * msg, size_t msgLength, int socketFd)
{
	int result = -1, n = -1, nReady = -1;
	uint32_t i;
	struct pollfd pollFds[2];
	char line_in[MAX_LINELEN], line_out[MAX_LINELEN];

	bzero(line_in, MAX_LINELEN);
	bzero(line_out, MAX_LINELEN);

	pollFds[0].fd = fileno(stdin);
	pollFds[0].events = POLLIN | POLLPRI;

	pollFds[1].fd = socketFd;
	pollFds[1].events = POLLIN | POLLPRI;

	for (;;)
	{
		nReady = poll(pollFds, 2, MAX_POLL_TIMEOUT);

		for (i = 0U; i < 2U; i++)
		{
			if ((pollFds[i].revents & (POLLRDNORM | POLLERR)) > 0)
			{
				DIAG_INFO("%s got a msg (%#x)!", (i>0)?"socket":"user interface", pollFds[i].revents);
				if (0 == i)
				{
					n = read(fileno(stdin), line_in, strlen(line_in));
					if (0 == n) /* And when user input is like "" */
					{
						shutdown();
					}
					n = write(socketFd, line_in, strlen(line_in)); /* For anything you want to write from user input to socket. */
					if (n <= 0)
					{
						DIAG_FATAL("Client: write failed (%d).\n", n);
					}
				}
				else
				{
					n = read(socketFd, line_out, MAX_LINELEN);
					if (n <= 0)
					{
						DIAG_ERROR("Client: EOF or Read failed (%d).\n", n);
					}
					else
					{
						struct user_info
						{
							char name[20];
							char email[30];
							uint8_t age;
						} userInfo;

						memcpy(userInfo.name, line_out, 20);
						memcpy(userInfo.email, line_out+20, 30);
						memcpy(&userInfo.age, line_out+50, 1);

						DIAG_INFO("Name: %s; Email: %s;Age: %d.\n", userInfo.name, userInfo.email, userInfo.age);
					}
				}
			}
		}
	}

	return STATUS_OK;
}

SYSTEM_STATUS server_msgSend()
{
	int fd_listen, fd_conn;
	struct sockaddr_in srv_addr, clt_addr;
	socklen_t clt_len;
	pid_t pid_child;
	char str_addr[INET_ADDRSTRLEN];

	fd_listen = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&srv_addr, sizeof(srv_addr));

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(9877);

	bind(fd_listen, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	listen(fd_listen, 10);
	//inet_ntop(AF_INET, &srv_addr.sin_addr.s_addr, str_addr, INET_ADDRSTRLEN);
	DIAG_INFO("Server: Starts.");
	for (;;)
	{
		DIAG_INFO("Server: Listening...");
		clt_len = sizeof(clt_addr);
		fd_conn = accept(fd_listen, (struct sockaddr *)&clt_addr, &clt_len);
		DIAG_INFO("Accepted.");
		if (0 == (pid_child = fork()))
		{
			close(fd_listen);
			str_echo(fd_conn);
			exit(0);
		}

		close(fd_conn);

	}
}

void str_echo(int fd)
{
	ssize_t n = -1;
	char buf[LINELEN_MAX];

	bzero(buf, LINELEN_MAX);

	while (1)
	{
		DIAG_INFO("Server: reading! \n");
		n = read(fd, buf, LINELEN_MAX);
		if (n > 0)
		{
			uint32_t result = 0;
			struct user_info
			{
				char name[20];
				char email[30];
				uint8_t age;
			} user_info_t;


			memcpy(user_info_t.name, buf, 20);
			memcpy(user_info_t.email, buf+20, 30);
			memcpy(&user_info_t.age, buf+50, 1);

			DIAG_INFO("Server - Name: %s; Email: %s; Age: %d.", user_info_t.name, user_info_t.email, user_info_t.age);
//			DIAG_INFO("Server: Received string %s (%d bytes)\n", buf, n);
			write(fd, buf, n);
		}
		else
		{
			DIAG_INFO("Server: EOF or failure by read()");
			break;
		}
	}

	if (n < 0)
	{
		DIAG_INFO("Server: Read failed. Errno = %#x\n", errno);
	}
}
#if 0
SYSTEM_STATUS writexxx()
{
	SYSTEM_STATUS result;
	int nLeft = -1;
	int nWritten = -1;

	/* Write n bytes from fd. It shall try reading even if the socket buffer limit is hit. */
	ssize_t write_ex(int socket_fd, void * buf, size_t n)
	{
		size_t nLeft = n;
		ssize_t nWritten = -1;
		ssize_t rt = -1;

		while (nLeft > 0)
		{
			nWritten = read(socket_fd, buf, nLeft);
			if (nWritten > 0)
			{
				/* Shall try again writing if socket buffer limit gets hit. */
				nLeft -= nWritten;
				buf += nWritten;
			}
			else
			{
				if ((nWritten < 0) && (EINTR == errno))
				{
					continue;
				}
				else
				{
					break;
				}
			}
		}

		/* nread can either be 0 (happy reading everything) or -1 (damm! error!)*/
		rt = (nWritten < 0) ? nWritten : n;

		return rt;
	}

	return result;
}


/* Read n bytes from fd. It shall try reading even if the socket buffer limit is hit. */
ssize_t read_ex(int fd, void * buf, size_t n)
{
	size_t nleft = n;
	ssize_t nread = -1;
	ssize_t rt = -1;

	while(nleft > 0)
	{
		nread = read(fd, buf, nleft);
		if (nread > 0)
		{
			/* Shall try again reading if socket buffer limit gets hit. */
			nleft -= nread;
			buf += nread;
		}
		else if (0 == nread)
		{
			/* Yes! Had read everything you can. */
			break;
		}
		else
		{
			if (EINTR == errno)
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}

	/* nread can either be 0 (happy reading everything) or -1 (damm! error!)*/
	rt = (0 == nread) ? (n - nleft) : rt;

	return rt;
}
#endif
