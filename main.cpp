#include <iostream>
#include "TcpServer.h"
#include "msg.h"

using namespace std;

int main()
{
	int res;
	
	messageQueueInit();

	TcpServer *pTcpServer = new TcpServer();

	pTcpServer->startTcpServer();

	return 0;
}
