#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "common.h"
#include "eLibrary.h"
#include "eLibrary_socket.h"


int main(int argc, char ** argv)
{
	int fd;
	struct sockaddr_in addr_srv;
	int result = -1;
	USER_INFO userInfo;

	/*
	strcpy(userInfo.name, "Murphy Meng");
	strcpy(userInfo.email, "singlaive@gmail.com");
	userInfo.numBooks = 3U;
	strcpy(userInfo.bookNames[0], "aaa");
	strcpy(userInfo.bookNames[1], "bbb");
	strcpy(userInfo.bookNames[2], "ccc");

	userInfoDump(&userInfo);
	result = syncServer(&userInfo);
*/
	exit(0);
}
