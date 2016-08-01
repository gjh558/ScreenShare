#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "ScreenServer.h"
#include "ScreenClient.h"

#define DEBUG 1

#ifdef DEBUG
	FILE *fp;
#endif
ScreenServer::ScreenServer(int sock, uint32_t id, uint32_t cid):
	mSockfd(sock),
	mId(id),
	mCId(cid)
{
	printf("a new ScreenServer created\n");
	#ifdef DEBUG
		fp = fopen("test.h264", "w+");
	#endif

	pBuffer = new uint8_t[1024 *  10];
	startThread();
}

ScreenServer::~ScreenServer()
{
	delete[] pBuffer;
}

void ScreenServer::startThread()
{
	int ret;
	pthread_t tid;

	ret = pthread_create(&tid, NULL, receiveLoop, this);
	if (ret != 0) {
		printf("create screen  server thread failed: %s\n", strerror(errno));
		exit(-1);
	}
}

void ScreenServer::startReceiveLoop()
{
	printf("screen server thread started\n");
	while (1) {
		//recv header
		//int ret = recvMessage(header, 8, pInstance->mSockfd);
		//if (ret < 0) {
		//	printf("recv message failed\n");
		//	return NULL;
		//}

		//parseHeader
		//recv payload
		uint64_t length = 0;
		int ret = recvMessage((uint8_t *) &length, sizeof(uint64_t), mSockfd);

		if (ret != sizeof(uint64_t)) {
			printf("recv video length from screen server failed\n");
			break;
		}
		printf("recv length = %ld\n", length);

		ret = recvMessage(pBuffer, length, mSockfd);
		if (ret != length)
		{
			printf("recv video data from screen server failed\n");
			break;
		}
		printf("recv frome server %s\n", pBuffer);
		#ifdef DEBUG
			fwrite(pBuffer, length, 1, fp);
		#endif
		//send to client
		if (mScreenClients.size() > 0) {
			printf("send to client\n");
			//char buf[] = "hello";
			vector<ScreenClient *>::iterator it;
			for (it = mScreenClients.begin(); it != mScreenClients.end(); it++){
				ScreenClient *pClient = *it;
				pClient->sendMessage(pBuffer, length, pClient->getSock());
			}
		}
	}

	printf("screen server %d exit\n", mId);
}

uint32_t ScreenServer::getCId()
{
	return mCId;
}

void ScreenServer::addClient(ScreenClient *client)
{
	mScreenClients.push_back(client);
}

void *ScreenServer::receiveLoop(void *args)
{
	uint8_t header[8];
	ScreenServer *pInstance = (ScreenServer *)args;

	pInstance->startReceiveLoop();

	return NULL;
}
