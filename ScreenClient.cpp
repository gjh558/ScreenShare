#include <stdio.h>
#include "ScreenClient.h"

ScreenClient::ScreenClient(int sock, uint32_t id, uint32_t cid):
	mSockfd(sock),
	mId(id),
	mCId(cid)
{
	printf("a new ScreenServer created\n");
}

ScreenClient::~ScreenClient()
{
}

uint32_t ScreenClient::getCId()
{
	return mCId;
}
