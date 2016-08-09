#include "RTSPSource.h"
#include <GroupsockHelper.hh> // for "gettimeofday()"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "ScreenServer.h"

//static uint8_t *bufPtr = buf;

static FILE *fp;

myDeviceSource*
myDeviceSource::createNew(UsageEnvironment& env, ScreenServer *server)
{
  fp = fopen("out.h264", "w+");
  return new myDeviceSource(env, server);
}

myDeviceSource*
myDeviceSource::createNew(UsageEnvironment& env, int sock)
{
  return new myDeviceSource(env, sock);
}

EventTriggerId myDeviceSource::eventTriggerId = 0;

unsigned myDeviceSource::referenceCount = 0;

myDeviceSource::myDeviceSource(UsageEnvironment& env, ScreenServer *server):
    pScreenServer(server),
    FramedSource(env)
{ 
  if (referenceCount == 0) 
  {
      
  }
  
  ++referenceCount;


  if (eventTriggerId == 0) 
  {
    eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
  }
}

myDeviceSource::myDeviceSource(UsageEnvironment& env, int sock):
    mSockfd(sock),
    FramedSource(env)
{ 
  if (referenceCount == 0) 
  {
      
  }
  
  ++referenceCount;


  if (eventTriggerId == 0) 
  {
    eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
  }
}

myDeviceSource::~myDeviceSource(void) {
  --referenceCount;
  envir().taskScheduler().deleteEventTrigger(eventTriggerId);
  eventTriggerId = 0;

  
  if (referenceCount == 0) 
  {
  }
}

void myDeviceSource::stopRTSPServer()
{
  pScreenServer->watchVal[0] = 1;
}

unsigned myDeviceSource::maxFrameSize() const
{
  return 102400;
}

int loop_count;

void myDeviceSource::doGetNextFrame() 
{
  int ret;
  //ret = pScreenServer->recvMessage((uint8_t *) &mFrameLength, sizeof(uint32_t), pScreenServer->getSockfd());

  // if (ret != sizeof(uint32_t)) {
  //   printf("recv video length from screen server failed\n");
  //   stopRTSPServer();
  //   return;
  // }
  //printf("recv length = %d\n", mFrameLength);

  // ret = pScreenServer->recvMessage((uint8_t *) &mFramePts, sizeof(uint32_t), pScreenServer->getSockfd());
  // if (ret != sizeof(uint32_t)) {
  //   printf("recv video pts from screen server failed\n");
  //   return;
  // }
   //printf("recv ...\n");

  ret = mBaseSocket.recvMessage(mFrameData, 2048, mSockfd);
  if (ret != 2048)
  {
    printf("close current rtsp server\n");
    stopRTSPServer();
    exit(-1);
  }

  mFrameLength = ret;
  //printf("mFrameLength = %d\n", mFrameLength);
  //fwrite(mFrameData, mFrameLength, 1, fp);
  deliverFrame();
}

void myDeviceSource::deliverFrame0(void* clientData) 
{
  ((myDeviceSource*)clientData)->deliverFrame();
}

void myDeviceSource::deliverFrame() 
{
  //if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

  u_int8_t* newFrameDataStart = (u_int8_t*) mFrameData;             //(u_int8_t*) buf; //%%% TO BE WRITTEN %%%
  unsigned newFrameSize = mFrameLength; //%%% TO BE WRITTEN %%%

  // Deliver the data here:
  if (newFrameSize > fMaxSize) {
    printf("#######################some error happened\n");
    fFrameSize = fMaxSize;
    fNumTruncatedBytes = newFrameSize - fMaxSize;
  } else {
    fFrameSize = newFrameSize;
  }
  //fDurationInMicroseconds = 20;
  gettimeofday(&fPresentationTime, NULL); 
  memmove(fTo, newFrameDataStart, fFrameSize);
  FramedSource::afterGetting(this);
}
