#ifndef SCREENSERVER_H
#define SCREENSERVER_H

#include <stdint.h>
#include <vector>

using namespace std;

class ScreenClient;

class ScreenServer{
public:
	ScreenServer(int sock, uint32_t id, uint32_t cid);
	~ScreenServer();

private:
	int mSockfd;
	uint32_t mId;
	uint32_t mCId;

	vector<ScreenClient *> mScreenClients;
};

#endif
