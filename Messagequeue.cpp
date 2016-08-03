#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "Messagequeue.h"

#define MSG_KEY 1234

static int msqid = 0;

int initMsgQueue()
{
	msqid = msgget(MSG_KEY, IPC_EXCL);
	if (msqid < 0) {
		msqid = msgget(MSG_KEY, IPC_CREAT | 0666);
		if (msqid < 0) {
			return -1;
		}
	}

	return msqid;
}

int uninitMsgQueue()
{
	msgctl(msqid, IPC_RMID, 0);

	return 0;
}

int sendMessage(Message *msg)
{
	int ret;
	ret = msgsnd(msqid, msg, sizeof(Message), IPC_NOWAIT);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int recvMessage(Message *msg)
{
	int ret;
	ret = msgrcv(msqid, msg, sizeof(Message), 0, 0);
	if (ret < 0) {
		return ret;
	}

	return 0;
}
