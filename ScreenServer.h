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

	uint32_t getCId();
	void addClient(ScreenClient *client);

private:
	int mSockfd;
	uint32_t mId;
	uint32_t mCId;

	vector<ScreenClient *> mScreenClients;
};

#endif
