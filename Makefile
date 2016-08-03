default:
	g++ Frame.cpp BaseSocket.cpp TcpServer.cpp ScreenClient.cpp ScreenServer.cpp msg.cpp main.cpp -o server -g -lpthread
