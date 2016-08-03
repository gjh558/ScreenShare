#ifndef MSG_Q_H
#define MSG_Q_H

#include <stdint.h>

#define MAX_MSG_LENGTH 1024 * 1024

struct Message {
	long msg_length;
	char msg_content[MAX_MSG_LENGTH];
};

//interfaces
int initMsgQueue();
int uninitMsgQueue();
int sendMessage(Message *msg);
int recvMessage(Message *msg);
#endif
