#include <iostream>
#include "TcpServer.h"

using namespace std;

int main()
{
	int res;
	
	TcpServer *pTcpServer = new TcpServer();

	pTcpServer->startTcpServer();

	return 0;
}
