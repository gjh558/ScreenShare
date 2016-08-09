#ifndef _MYDEVICE_SOURCE_HH
#define _MYDEVICE_SOURCE_HH

#include <FramedSource.hh>
#include <queue>
#include "Frame.h"
#include "BaseSocket.h"

class ScreenServer;

class myDeviceSource: public FramedSource {
public:
  static myDeviceSource* createNew(UsageEnvironment& env, ScreenServer *server);
  static myDeviceSource* createNew(UsageEnvironment& env, int sock);

public:
  static EventTriggerId eventTriggerId;
  // Note that this is defined here to be a static class variable, because this code is intended to illustrate how to
  // encapsulate a *single* device - not a set of devices.
  // You can, however, redefine this to be a non-static member variable.

  unsigned maxFrameSize() const;
protected:
  myDeviceSource(UsageEnvironment& env, ScreenServer *server);
  myDeviceSource(UsageEnvironment& env, int sock);
  // called only by createNew(), or by subclass constructors
  virtual ~myDeviceSource();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  //virtual void doStopGettingFrames(); // optional

private:
  static void deliverFrame0(void* clientData);
  void deliverFrame();

  void stopRTSPServer();

private:
  static unsigned referenceCount; // used to count how many instances of this class currently exist

private:
  uint32_t mFrameLength;
  uint8_t mFrameData[1024 * 1024];
  uint32_t mFramePts;

  ScreenServer *pScreenServer;

  int mSockfd;
  BaseSocket mBaseSocket;
};

#endif
