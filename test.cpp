#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
 #include <sys/types.h>
#include <sys/socket.h>
#include "BaseSocket.h"
#include "TcpServer.h"
#include "RTSPSource.h"


UsageEnvironment* env;
RTPSink* videoSink;


static void parseSideType(uint8_t *buffer,  uint32_t length, uint32_t & side_type, uint32_t & id, uint32_t & cid)
{
	int *p = (int *)buffer;

	side_type = p[0];
	id = p[1];
	cid = p[2];
}

static void play(int sock)
{
    // Open the input file as with Device as the source:
    myDeviceSource *devSource
        = myDeviceSource::createNew(*env, sock);
    if (devSource == NULL) 
    {

          *env << "Unable to read from\"" << "Buffer"
           << "\" as a byte-stream source\n";
          exit(1);
    }

    FramedSource* videoES = devSource;

    // Create a framer for the Video Elementary Stream:
    H264VideoStreamFramer* videoSource = H264VideoStreamFramer::createNew(*env, videoES,False);

    // Finally, start playing:
    *env << "Beginning to read from UPP...\n";
    videoSink->startPlaying(*videoSource, NULL, videoSink);
}

int main(int argc, char *argv[])
{
	uint32_t id, cid;

	int sockfd;

	BaseSocket baseSocket;

	if (argc < 2) {
		printf("Invalid argv\n");
		exit(-1);
	}

	sockfd = atoi(argv[1]);
	id = atoi(argv[2]);
	cid = atoi(argv[3]);

	
	printf("sockfd = %d, id = %d, roomid = %d\n", sockfd, id, cid);
	
	uint32_t status = SUCCESS;
	int res = baseSocket.sendMessage((uint8_t *)&status, sizeof(uint32_t), sockfd);
	

	// start rtsp server
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	// Create 'groupsocks' for RTP and RTCP:
	struct in_addr destinationAddress;
	destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
	// Note: This is a multicast address.  If you wish instead to stream
	// using unicast, then you should use the "testOnDemandRTSPServer"
	// test program - not this test program - as a model.

	const unsigned short rtpPortNum = 18888;
	const unsigned short rtcpPortNum = rtpPortNum+1;
	const unsigned char ttl = 255;

	const Port rtpPort(rtpPortNum);
	const Port rtcpPort(rtcpPortNum);

	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
	rtpGroupsock.multicastSendOnly(); // we're a SSM source
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
	rtcpGroupsock.multicastSendOnly(); // we're a SSM source

	// Create a 'H264 Video RTP' sink from the RTP 'groupsock':
	OutPacketBuffer::maxSize = 600000;
	videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

	// Create (and start) a 'RTCP instance' for this RTP sink:
	const unsigned estimatedSessionBandwidth = 5000; // in kbps; for RTCP b/w share
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen+1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0'; // just in case
	RTCPInstance* rtcp
	= RTCPInstance::createNew(*env, &rtcpGroupsock,
	              estimatedSessionBandwidth, CNAME,
	              videoSink, NULL /* we're a server */,
	              True /* we're a SSM source */);
	// Note: This starts RTCP running automatically

	RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554 + cid);
	if (rtspServer == NULL) {
	  *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
	  exit(1);
	}


	char url_name[32];
	sprintf(url_name, "%d", cid);
	ServerMediaSession* sms
	  = ServerMediaSession::createNew(*env, url_name,"UPP Buffer" ,
	         "Session streamed by \"testH264VideoStreamer\"",
	                     True /*SSM*/);
	sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
	rtspServer->addServerMediaSession(sms);

	char* url = rtspServer->rtspURL(sms);
	*env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;

	// Start the streaming:
	*env << "Beginning streaming...\n";
	play(sockfd);

	env->taskScheduler().doEventLoop(); // does not return

	printf("Exit\n");
	return 0;
}
