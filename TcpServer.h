#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stdint.h>
#include <vector>

using namespace std;

#define PORT			12345
#define MAX_CONNECTION	30

#define SCREENSERVER 1
#define SCREENCLIENT 2

/*
 * TcpServer is responsible for accept connection from ScreenServer. (maybe also ScreenClient if use TCP)
 * There is only one TcpServer instance int the process.
 */
class ScreenServer;

class TcpServer{
public:
	TcpServer();
	~TcpServer();

	void startTcpServer();

private:
	void init();
	void connectionHandlerLoop();
	void parseSideType(uint8_t *buffer,  uint32_t length, uint32_t & side_type, uint32_t & id, uint32_t & cid);

	int sendMessage(const unsigned char *buffer_void, uint64_t buffer_size);
	int recvMessage(void *buffer_void, uint64_t buffer_size);
private:
	vector<ScreenServer *> mScreenServers;
	int mSocketfd;
		
};


#endif