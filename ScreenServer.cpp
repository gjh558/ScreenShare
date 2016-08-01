#include <stdio.h>
#include "ScreenServer.h"

ScreenServer::ScreenServer(int sock, uint32_t id, uint32_t cid):
	mSockfd(sock),
	mId(id),
	mCId(cid)
{
	printf("a new ScreenServer created\n");
}

ScreenServer::~ScreenServer()
{
}

uint32_t ScreenServer::getCId()
{
	return mCId;
}

void ScreenServer::addClient(ScreenClient *client)
{
	mScreenClients.push_back(client);
}
