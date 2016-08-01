#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "BaseSocket.h"

uint64_t BaseSocket::message_xmit(int sockfd, void *buff, uint64_t bufflen, int sending)
{
	uint64_t total = 0;

	if (!bufflen)
		return 0;

	do {
		uint64_t nbytes;

		if (sending)
			nbytes = send(sockfd, buff, bufflen, 0);
		else
			nbytes = recv(sockfd, buff, bufflen, MSG_WAITALL);

		if (nbytes <= 0)
			return -1;

		buff	= (void *) ((intptr_t) buff + nbytes);
		bufflen	-= nbytes;
		total	+= nbytes;

	} while (bufflen > 0);


	return total;
}

uint64_t BaseSocket::sendMessage(void *buff, uint64_t bufflen, int sockfd)
{
	return message_xmit(sockfd, buff, bufflen, 1);
}

uint64_t BaseSocket::recvMessage(void *buff, uint64_t bufflen, int sockfd)
{
	return message_xmit(sockfd, buff, bufflen, 0);
}
