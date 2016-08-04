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

myDeviceSource*
myDeviceSource::createNew(UsageEnvironment& env, ScreenServer *server)
{
  return new myDeviceSource(env, server);
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

myDeviceSource::~myDeviceSource(void) {
  --referenceCount;
  envir().taskScheduler().deleteEventTrigger(eventTriggerId);
  eventTriggerId = 0;

  
  if (referenceCount == 0) 
  {
  }
}

int loop_count;

void myDeviceSource::doGetNextFrame() 
{
	printf("get next frame\n");

  int ret = pScreenServer->recvMessage((uint8_t *) &mFrameLength, sizeof(uint32_t), pScreenServer->getSockfd());

  if (ret != sizeof(uint32_t)) {
    printf("recv video length from screen server failed\n");
    return;
  }
  printf("recv length = %d\n", mFrameLength);

  ret = pScreenServer->recvMessage(mFrameData, mFrameLength, pScreenServer->getSockfd());
  if (ret != mFrameLength)
  {
    printf("recv video data from screen server failed\n");
    return;
  }

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
    fFrameSize = fMaxSize;
    fNumTruncatedBytes = newFrameSize - fMaxSize;
  } else {
    fFrameSize = newFrameSize;
  }
  gettimeofday(&fPresentationTime, NULL); 
  memmove(fTo, newFrameDataStart, fFrameSize);
  FramedSource::afterGetting(this);
}
