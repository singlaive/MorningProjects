#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> /* For fork! */
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 
#include <errno.h>

#define LINELEN_MAX 80U

void str_echo(int fd);

int main(int argc, char ** argv)
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
	printf("Server: Starts.\n");
	for (;;)
	{
		printf("Server: Listening...\n");
		clt_len = sizeof(clt_addr);
		fd_conn = accept(fd_listen, (struct sockaddr *)&clt_addr, &clt_len);
		printf("Accepted.\n");
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
	//	printf("Server: reading! \n");
		n = read(fd, buf, LINELEN_MAX);
		if (n > 0)
		{
			printf("Server: Received string %s (%d bytes)\n", buf, n);		
			write(fd, buf, n);
		}
		else
		{
			printf("Server: EOF or failure by read()");
			break;
		}
	}

	if (n < 0)
	{
		printf("Server: Read failed. Errno = %#x\n", errno);
	}
}
