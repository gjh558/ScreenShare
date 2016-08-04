#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "ScreenServer.h"
#include "ScreenClient.h"
#include "Frame.h"
#include "msg.h"

#define DEBUG 1

#ifdef DEBUG
	FILE *fp;
#endif
ScreenServer::ScreenServer(int sock, uint32_t id, uint32_t cid):
	mSockfd(sock),
	mId(id),
	mCId(cid)
{
	printf("a new ScreenServer created\n");
	#ifdef DEBUG
		fp = fopen("test.h264", "w+");
	#endif

	pBuffer = new uint8_t[1024 *  1024];
	startThread();
}

ScreenServer::~ScreenServer()
{
	delete[] pBuffer;
}

int ScreenServer::getSockfd()
{
	return mSockfd;
}

void ScreenServer::startThread()
{
	int ret;
	pthread_t tid1, tid2;

	// ret = pthread_create(&tid1, NULL, receiveLoop, this);
	// if (ret != 0) {
	// 	printf("create screen  server recv thread failed: %s\n", strerror(errno));
	// 	exit(-1);
	// }

	ret = pthread_create(&tid2, NULL, sendLoop, this);
	if (ret != 0) {
		printf("create screen  server send thread failed: %s\n", strerror(errno));
		exit(-1);
	}
}

void ScreenServer::startSendLoop()
{

		// if (mFrames.size() > 0 && mScreenClients.size() > 0) {
			
		// 		Frame *pFrame = mFrames.front();
		// 		mFrames.pop();
		// 		vector<ScreenClient *>::iterator it;
		// 		for (it = mScreenClients.begin(); it != mScreenClients.end(); it++){
		// 			ScreenClient *pClient = *it;
		// 			pClient->sendMessage((uint8_t *) &(pFrame->size), sizeof(uint32_t), pClient->getSock());
		// 			pClient->sendMessage(pFrame->data, pFrame->size, pClient->getSock());
		// 		}
		// }
		//send to client
		// if (mScreenClients.size() > 0) {
		// 	printf("send to client\n");
		// 	//char buf[] = "hello";
		// 	vector<ScreenClient *>::iterator it;
		// 	for (it = mScreenClients.begin(); it != mScreenClients.end(); it++){
		// 		ScreenClient *pClient = *it;
		// 		pClient->sendMessage((uint8_t *) &length, sizeof(uint32_t), pClient->getSock());
		// 		pClient->sendMessage(pBuffer, length, pClient->getSock());
		// 	}
		// }

		// Begin by setting up our usage environment:
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
  const unsigned estimatedSessionBandwidth = 1024; // in kbps; for RTCP b/w share
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

  RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }
  ServerMediaSession* sms
    = ServerMediaSession::createNew(*env, "ipcamera","UPP Buffer" ,
           "Session streamed by \"testH264VideoStreamer\"",
                       True /*SSM*/);
  sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
  rtspServer->addServerMediaSession(sms);

  char* url = rtspServer->rtspURL(sms);
  *env << "Play this stream using the URL \"" << url << "\"\n";
  delete[] url;

  // Start the streaming:
  *env << "Beginning streaming...\n";
  play();

  env->taskScheduler().doEventLoop(); // does not return
}

void ScreenServer::startReceiveLoop()
{
	printf("screen server thread started\n");
	Message m;
	while (1) {
		//recv header
		//int ret = recvMessage(header, 8, pInstance->mSockfd);
		//if (ret < 0) {
		//	printf("recv message failed\n");
		//	return NULL;
		//}

		//parseHeader
		//recv payload
		uint32_t length;
		int ret = recvMessage((uint8_t *) &length, sizeof(uint32_t), mSockfd);

		if (ret != sizeof(uint32_t)) {
			printf("recv video length from screen server failed\n");
			break;
		}
		printf("recv length = %d\n", length);

		ret = recvMessage(pBuffer, length, mSockfd);
		if (ret != length)
		{
			printf("recv video data from screen server failed\n");
			break;
		}
		//printf("recv frome server %s\n", pBuffer);
		// #ifdef DEBUG
		// 	fwrite(pBuffer, length, 1, fp);
		// #endif

		
		if (mFrames.size() < CACHE_FRAME_NUM) {
			Frame *aFrame = new Frame(pBuffer, length);
			mFrames.push(aFrame);
		} else {
			mFrames.pop();
			Frame *aFrame = new Frame(pBuffer, length);
			mFrames.push(aFrame);
		}

		//send to client
		// if (mScreenClients.size() > 0) {
		// 	printf("send to client\n");
		// 	//char buf[] = "hello";
		// 	vector<ScreenClient *>::iterator it;
		// 	for (it = mScreenClients.begin(); it != mScreenClients.end(); it++){
		// 		ScreenClient *pClient = *it;
		// 		pClient->sendMessage((uint8_t *) &length, sizeof(uint32_t), pClient->getSock());
		// 		pClient->sendMessage(pBuffer, length, pClient->getSock());
		// 	}
		// }
		
	}

	printf("screen server %d exit\n", mId);
}

uint32_t ScreenServer::getCId()
{
	return mCId;
}

void ScreenServer::addClient(ScreenClient *client)
{
	mScreenClients.push_back(client);
}

void *ScreenServer::receiveLoop(void *args)
{
	uint8_t header[8];
	ScreenServer *pInstance = (ScreenServer *)args;

	pInstance->startReceiveLoop();

	return NULL;
}

void *ScreenServer::sendLoop(void *args)
{
	uint8_t header[8];
	ScreenServer *pInstance = (ScreenServer *)args;

	pInstance->startSendLoop();

	return NULL;
}


void ScreenServer::afterPlaying(void* clientData) 
{

    play();
}
//------------------------------------------------------------------------
//PLAY FUNCTION () 
//------------------------------------------------------------------------
void ScreenServer::play()
{


      // Open the input file as with Device as the source:
    myDeviceSource* devSource
        = myDeviceSource::createNew(*env, this);
    if (devSource == NULL) 
    {

          *env << "Unable to read from\"" << "Buffer"
           << "\" as a byte-stream source\n";
          exit(1);
    }

    FramedSource* videoES = devSource;

    // Create a framer for the Video Elementary Stream:
    videoSource = H264VideoStreamFramer::createNew(*env, videoES,False);

    // Finally, start playing:
    *env << "Beginning to read from UPP...\n";
    videoSink->startPlaying(*videoSource, NULL, videoSink);
}