#include "TcpServer.h"
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

#include "ScreenServer.h"
#include "ScreenClient.h"

TcpServer::TcpServer()
{
}

TcpServer::~TcpServer()
{
}

/*
 * This is a loop, exit with the process
 */
void TcpServer::startTcpServer()
{
	//initialize tcp server
	init();

	//start accept loop
	connectionHandlerLoop();
}

void TcpServer::init()
{
	int opt = 1;
	struct sockaddr_in server_sockaddr;

	mSocketfd = socket(AF_INET,SOCK_STREAM, 0);
	if (mSocketfd <= 0) {
		printf("init server socket failed: %s\n", strerror(errno));
		exit(0);
	}
	setsockopt(mSocketfd, SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	///bind，成功返回0，出错返回-1
	if(bind(mSocketfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
	{
	    perror("bind failed");
	    exit(1);
	}

	///listen，成功返回0，出错返回-1
	if(listen(mSocketfd, MAX_CONNECTION) == -1)
	{
	    perror("listen failed");
	    exit(1);
	}
}

int TcpServer::sendMessage(const unsigned char *buffer_void, uint64_t buffer_size, int sockfd)
{
	const unsigned char *buffer = buffer_void;

	/* default is success */
	int retval = 0;

	/* how many bytes remains to be written */
	size_t remaining = buffer_size;

	/* repeat until all data is written */
	while (remaining > 0)
	{
		fd_set write_fd;
		int selret;

		FD_ZERO(&write_fd);
		FD_SET(sockfd, &write_fd);

		selret = select(sockfd + 1, NULL, &write_fd, NULL, NULL);

		/* try to write only when the file descriptor is writable */
		if (selret > 0)
		{
			int written;

			if (!FD_ISSET(sockfd, &write_fd))
			{
				/* very strange situation. it should be an assert really */
				retval = -1;
				break;
			}
			/* since we are a user library we can't play with signals
			 * The signals may already be used by the application */
			/* Get EPIPE return code instead of SIGPIPE signal
			 * Works on Linux */
			written = send(sockfd, buffer, remaining, MSG_NOSIGNAL);

			if (written > 0)
			{
				/* we wrote something */
				buffer += written;
				remaining -= written;
			} else if (written == 0)
			{
				/* peer closed the socket */
				retval = -1;
				break;
			} else
			{
				/* we ignore the signals and socket full situations, all
				 * other errors are fatal */
				if (errno != EINTR && errno != EAGAIN)
				{
					retval = -1;
					break;
				}
			}
		} else if (selret == 0)
		{
			/* timeout */
			retval = -2;
			break;
		} else
		{
			/* ignore signals */
			if (errno != EINTR)
			{
				printf("select returns with failure: %s\n",
					strerror(errno));
				retval = -1;
				break;
			}
		}
	}

	return retval;
}

int TcpServer::recvMessage(void *buffer_void, uint64_t buffer_size, int sockfd)
{
	char *buffer = (char *)buffer_void;

	/* default is success */
	int retval = 0;

	/* how many bytes we must read */
	size_t remaining = buffer_size;

	/* repeat until we get the whole message */
	while (remaining > 0)
	{
		fd_set read_fd;
		int selret;

		FD_ZERO(&read_fd);
		FD_SET(sockfd, &read_fd);

		selret = select(sockfd + 1, &read_fd, NULL, NULL, NULL);

		/* try to read only when socket is readable */
		if (selret > 0)
		{
			int readed;

			if (!FD_ISSET(sockfd, &read_fd))
			{
				/* very strange situation. it should be an assert really */
				retval = -1;
				break;
			}
			readed = read(sockfd, buffer, remaining);

			if (readed > 0)
			{
				/* we got something */
				buffer += readed;
				remaining -= readed;
			} else if (readed == 0)
			{
				/* peer closed the socket */
				retval = 1;
				break;
			} else
			{
				/* we ignore the signals and empty socket situations, all
				 * other errors are fatal */
				if (errno != EINTR && errno != EAGAIN)
				{
					retval = -1;
					break;
				}
			}
		}
		else
		{
			/* we ignore signals, all other errors are fatal */
			if (errno != EINTR)
			{
				retval = -1;
				break;
			}
		}
	}

	return retval;
}

//add client to correct screenserver based on class id
void TcpServer::addClient2Server(ScreenClient *client)
{
	uint32_t client_cid = client->getCId();

	vector<ScreenServer *>::iterator it;
	for (it = mScreenServers.begin(); it != mScreenServers.end(); it++)
	{
		uint32_t server_cid = (*it)->getCId();
		if (client_cid == server_cid) {
			(*it)->addClient(client);
		}
	}
	
}

void TcpServer::parseSideType(uint8_t *buffer,  uint32_t length, uint32_t & side_type, uint32_t & id, uint32_t & cid)
{
	int *p = (int *)buffer;

	side_type = p[0];
	id = p[1];
	cid = p[2];
}

void TcpServer::connectionHandlerLoop()
{
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);

	uint8_t buffer[12];
	uint32_t side_type,
			 id,
			 cid;

	while (1) {
		if (mScreenServers.size() < MAX_CONNECTION) {
			int sockfd = accept(mSocketfd, (struct sockaddr*)&client_addr, &length);
			if (sockfd > 0) {
				printf("new connection created\n");

				int res = recvMessage(buffer, 12, sockfd);
				if (res < 0) {
					printf("recv Message failed\n");
					exit(-1);
				}

				parseSideType(buffer, 12, side_type, id, cid);
				printf("side_type = %d, id = %d, cid = %d\n", side_type, id, cid);

				if (side_type == SCREENSERVER) {
					//this connection is from screenserver, so we create a screenserver instance here.
					ScreenServer *instance = new ScreenServer(sockfd, id, cid);
					mScreenServers.push_back(instance);
				} else if (side_type == SCREENCLIENT) {
					ScreenClient *instance = new ScreenClient(sockfd, id, cid);
					addClient2Server(instance);
				}
				
			}else {
				perror("accept error.");
			}

		}
	}
}
