#ifndef SCREENSERVER_H
#define SCREENSERVER_H

#include <stdint.h>
#include <vector>
#include <queue>
#include "BaseSocket.h"

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include "RTSPSource.h"

using namespace std;

#define CACHE_FRAME_NUM 25

class Frame;
class ScreenClient;

class ScreenServer : public BaseSocket {
public:
	ScreenServer(int sock, uint32_t id, uint32_t cid);
	~ScreenServer();

	uint32_t getCId();
	void addClient(ScreenClient *client);
	void startReceiveLoop();
	void startSendLoop();
	static void *receiveLoop(void *);
	static void *sendLoop(void *);

	int getSockfd();

	volatile char watchVal[1];
private:
	void startThread();
	int parseHeader(uint8_t *header);
private:
	int mSockfd;
	uint32_t mId;
	uint32_t mCId;
	uint8_t *pBuffer;
	vector<ScreenClient *> mScreenClients;
	queue<Frame *> mFrames;

private:
	UsageEnvironment* env;
	H264VideoStreamFramer* videoSource;
	RTPSink* videoSink;
	myDeviceSource* devSource;

	void play();
	void afterPlaying(void* /*clientData*/);
};

#endif
