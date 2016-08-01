#include "ScreenServer.h"

ScreenServer::ScreenServer(int sock, uint32_t id, uint32_t cid):
	mSockfd(sock),
	mId(id),
	mCId(cid)
{
}

ScreenServer::~ScreenServer()
{
}


