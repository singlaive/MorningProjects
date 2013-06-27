#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "../../common/inc/common.h"
#include "../inc/eLibrary.h"


int main(int argc, char ** argv)
{
	int fd;
	struct sockaddr_in addr_srv;
	int result = -1;
	USER_INFO userInfo;

	//userInfoDump(&userInfo);

	DIAG_FATAL("sizeof userInfo = %d", sizeof(userInfo));

	//result = syncServer(&userInfo);



	exit(0);
}

SYSTEM_STATUS syncServer(USER_INFO * userInfo)
{
	char line_in[LINELEN_MAX], line_out[LINELEN_MAX];
	int n = -1;
	SYSTEM_STATUS result = STATUS_FAIL;
	int socketFd = -1;

	socketFd = client_socketConnInit();

	result = client_socketDataUpdate(userInfo, socketFd);
	assert(result == STATUS_OK);

	return result;
}

void userInfoDump(USER_INFO * userInfo)
{
	uint8_t i = 0U;
	struct bookList	* current = userInfo->books;

	DIAG_INFO("User info");
	DIAG_INFO("    name: %s", userInfo->username);
	DIAG_INFO("    email: %s", userInfo->email);
	DIAG_INFO("    num of books: %d", userInfo->numBooks);
	
	while (NULL != current)
	{
		DIAG_INFO("    book[%d]: %s", i, current->book->name);
		current = current->next;
		i++;
	}
}
