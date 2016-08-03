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

//static uint8_t *bufPtr = buf;

myDeviceSource*
myDeviceSource::createNew(UsageEnvironment& env)
{
  return new myDeviceSource(env);
}

EventTriggerId myDeviceSource::eventTriggerId = 0;

unsigned myDeviceSource::referenceCount = 0;

myDeviceSource::myDeviceSource(UsageEnvironment& env):FramedSource(env) 
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
    //for (loop_count=0; loop_count < 13; loop_count++)
    //{
  Message m;
  messageQueueRecv(&m);

  
  uint32_t *pTmp = (uint32_t *)(m.m_text);

  mFrameLength = *pTmp;
  printf("recved Message. length = %d\n", mFrameLength);
  memcpy(mFrameData, m.m_text + 4, mFrameLength);

  deliverFrame();
}

void myDeviceSource::deliverFrame0(void* clientData) 
{
  ((myDeviceSource*)clientData)->deliverFrame();
}

void myDeviceSource::deliverFrame() 
{
  if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

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
