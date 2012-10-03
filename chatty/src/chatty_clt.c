#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include "../inc/dummy.h"

#define LINELEN_MAX 80U

void take_cmd(FILE *fp, int fd);

int main(void)
{
	int fd;
	struct sockaddr_in addr_srv;
	
	struct user_info
	{
		char name[20];
		char email[30];
		uint8_t age;
	} user_info_t;

	puts("Name: ");
	fgets(user_info_t.name, 20, stdin);
	puts("Email: ");
	fgets(user_info_t.email, 30, stdin);
//	puts("age: ");
//	fgets(user_info_t.age, 1, stdin);
	user_info_t.age = 30;

	printf("Name: %s; Email: %s;Age: %d.", user_info_t.name, user_info_t.email, user_info_t.age);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&addr_srv, sizeof(addr_srv));

	addr_srv.sin_family = AF_INET;
	addr_srv.sin_port = htons(9877);

	//inet_pton(AF_INET, argv[1], &addr_srv.sin_addr);

	connect(fd, (struct sockaddr *)&addr_srv, sizeof(addr_srv));

	take_cmd(stdin, fd);

	exit(0);
}

void take_cmd(FILE *fp, int fd)
{
	char line_in[LINELEN_MAX], line_out[LINELEN_MAX];
	int n = -1;

	bzero(line_in, LINELEN_MAX);

	while (NULL != fgets(line_in, LINELEN_MAX, fp))
	{
		bzero(line_out, LINELEN_MAX);
		n = write(fd, line_in, strlen(line_in));
		if (n <= 0)
		{
			printf("Client: write failed (%d).\n", n);
			break;
		}
		else
		{
			n = read(fd, line_out, LINELEN_MAX);
			if (n <= 0)
			{
				printf("Client: EOF or Read failed (%d).\n", n);
				break;
			}
			else
			{
				printf("Client: Received string \"%s\" (length %d, really? %d)\n", line_out, n, strlen(line_out));
			//printf("Client: [0]= %c; [1]= %c; [2] = %c\n", line_out[0], line_out[1], line_out[2]);
			}
/*		fputs(line_out, stdout);*/
		}
	}
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


/* Write n bytes from fd. It shall try reading even if the socket buffer limit is hit. */
ssize_t write_ex(int fd, void * buf, size_t n)
{
	size_t nleft = n;
	ssize_t nwritten = -1;
	ssize_t rt = -1;

	while (nleft > 0)
	{
		nwritten = read(fd, buf, nleft);
		if (nwritten > 0)
		{
			/* Shall try again writing if socket buffer limit gets hit. */
			nleft -= nwritten;
			buf += nwritten;
		}
		else
		{
			if ((nwritten < 0) && (EINTR == errno))
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
	rt = (nwritten < 0) ? nwritten : n;

	return rt;
}
