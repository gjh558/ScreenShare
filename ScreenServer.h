#ifndef SCREENSERVER_H
#define SCREENSERVER_H

#include <stdint.h>
#include <vector>
#include "BaseSocket.h"

using namespace std;

class ScreenClient;

class ScreenServer : public BaseSocket {
public:
	ScreenServer(int sock, uint32_t id, uint32_t cid);
	~ScreenServer();

	uint32_t getCId();
	void addClient(ScreenClient *client);
	void startReceiveLoop();
	static void *receiveLoop(void *);
private:
	void startThread();
	int parseHeader(uint8_t *header);
private:
	int mSockfd;
	uint32_t mId;
	uint32_t mCId;
	uint8_t *pBuffer;
	vector<ScreenClient *> mScreenClients;
};

#endif
