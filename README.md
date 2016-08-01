# ScreenShare
BUG1:
	screenserver must connect to TCPServer ahead of screenclient. bug is in TcpServer::addClient2Server()
