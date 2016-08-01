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

//add client to correct screenserver based on class id
void TcpServer::addClient2Server(ScreenClient *client)
{
	uint32_t client_cid = client->getCId();
	printf("Cliend cid = %d\n", client_cid);

	vector<ScreenServer *>::iterator it;
	for (it = mScreenServers.begin(); it != mScreenServers.end(); it++)
	{
		uint32_t server_cid = (*it)->getCId();
		printf("server cid = %d\n", server_cid);
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
				if (res != 12) {
					printf("recv Message failed\n");
					exit(-1);
				}

				parseSideType(buffer, 12, side_type, id, cid);
				printf("side_type = %d, id = %d, cid = %d\n", side_type, id, cid);

				if (side_type == SCREENSERVER) {
					//this connection is from screenserver, so we create a screenserver instance here.
					ScreenServer *instance = new ScreenServer(sockfd, id, cid);
					mScreenServers.push_back(instance);
					//res = sendMessage();
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
