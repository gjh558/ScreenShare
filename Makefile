default:
	g++ Frame.cpp BaseSocket.cpp TcpServer.cpp ScreenClient.cpp ScreenServer.cpp main.cpp -o server -g -lpthread
