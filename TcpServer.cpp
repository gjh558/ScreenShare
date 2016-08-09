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
#include "Room.h"

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
int TcpServer::addClient2Server(ScreenClient *client)
{
	uint32_t client_cid = client->getCId();
	printf("Cliend cid = %u\n", client_cid);

	
	vector<ScreenServer *>::iterator it;
	for (it = mScreenServers.begin(); it != mScreenServers.end(); it++)
	{
		//printf("screenserver = %p\n", (*it));
		ScreenServer *pServer = (*it);
		uint32_t server_cid = pServer->getCId();
		printf("server cid = %d\n", server_cid);
		if (client_cid == server_cid) {
			pServer->addClient(client);
			return 0;
		}
	}
	
	return -1;
}

void TcpServer::parseSideType(uint8_t *buffer,  uint32_t length, uint32_t & side_type, uint32_t & id, uint32_t & cid)
{
	int *p = (int *)buffer;

	side_type = p[0];
	id = p[1];
	cid = p[2];
}

int TcpServer::isValidRoom(uint32_t id, uint32_t rid)
{
	vector<Room *>::iterator it;
	for (it = mRooms.begin(); it != mRooms.end(); it++) {
		Room *p = *it;
		if (p->teacherId == id || p->roomId == rid) {
			return -1;
		}
	}

	return 0;
}

void TcpServer::connectionHandlerLoop()
{
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);

	uint8_t buffer[12];
	uint32_t side_type,
			 id,
			 cid;
	pid_t pid;

	while (1) {
		int sockfd = accept(mSocketfd, (struct sockaddr*)&client_addr, &length);
		if (sockfd > 0) {
			printf("new connection created\n"); 

			int res = recvMessage(buffer, 12, sockfd);
			if (res != 12) {
				printf("recv Message failed\n");
				exit(-1);
			}

			parseSideType(buffer, 12, side_type, id, cid);
			printf("id = %d, roomid = %d\n", id, cid);

			uint32_t status;
			if (side_type == SCREENSERVER) {
			// 	//this connection is from screenserver, so we create a screenserver instance here.
			 	// if (isValidRoom(id, cid) == 0) {
					// Room *aroom = new Room(id, cid);
					// mRooms.push_back(aroom);

					if ((pid = fork()) < 0) {
						printf("cannot create process for new connection\n");
						status = FAILED;
						res = sendMessage((uint8_t *)&status, sizeof(uint32_t), sockfd);
						exit(-1);
					} else if (pid == 0) {
						//close(mSocketfd);
						char argv[3][8];
						sprintf(argv[0], "%d", sockfd);
						sprintf(argv[1], "%d", id);
						sprintf(argv[2], "%d", cid);
						printf("start rtsp server\n");
						execl("rtspserver",  "rtspserver", argv[0], argv[1], argv[2], NULL);
					}

					
				// } else {
				// 	status = FAILED;
				// 	res = sendMessage((uint8_t *)&status, sizeof(uint32_t), sockfd);
				// }

			} else if (side_type == SCREENCLIENT) {
				ScreenClient *instance = new ScreenClient(sockfd, id, cid);
				res = addClient2Server(instance);
				if (res < 0) {
					uint32_t status = FAILED;
					res = sendMessage((uint8_t *)&status, sizeof(uint32_t), sockfd);
					close(sockfd);
				} else {
					uint32_t status = SUCCESS;
					res = sendMessage((uint8_t *)&status, sizeof(uint32_t), sockfd);
				}
			}
			
		}else {
			perror("accept error.");
		}
	}
}
