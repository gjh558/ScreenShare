#ifndef BASESOCKET_H
#define BASESOCKET_H

class BaseSocket {
public:
	uint64_t sendMessage(void *buff, uint64_t bufflen, int sockfd);
	uint64_t recvMessage(void *buff, uint64_t bufflen, int sockfd);
private:
	uint64_t message_xmit(int sockfd, void *buff, uint64_t bufflen, int sending);
};

#endif
