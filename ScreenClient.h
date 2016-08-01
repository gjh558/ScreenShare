#ifndef SCREENCLIENT_H
#define SCREENCLIENT_H

#include <stdint.h>
#include "BaseSocket.h"

class ScreenClient : public BaseSocket {
public:
	ScreenClient(int sock, uint32_t id, uint32_t cid);
	~ScreenClient();

	uint32_t getCId();
	int getSock();
private:
	int mSockfd;
	uint32_t mId;
	uint32_t mCId;
};

#endif
