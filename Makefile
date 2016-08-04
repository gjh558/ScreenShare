default:
	g++ Frame.cpp RTSPSource.cpp BaseSocket.cpp TcpServer.cpp ScreenClient.cpp ScreenServer.cpp msg.cpp main.cpp \
		-I/usr/local/include/BasicUsageEnvironment \
	    -I/usr/local/include/groupsock \
	    -I/usr/local/include/liveMedia \
	    -I/usr/local/include/UsageEnvironment \
	    -L/usr/local/lib \
	    -lliveMedia \
	    -lBasicUsageEnvironment \
	    -lgroupsock \
	    -lUsageEnvironment \
	    -o server -g -lpthread
