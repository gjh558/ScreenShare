#include "msg.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>

static int msgid = -1;

int messageQueueInit()
{
	key_t key = -1;

	key = ftok(KEY_NAME, 1);
	if (key == -1)
	{
		printf("ftok\n");
		return -1;
	}

	//signal(SIGINT, sig_catch_int);

	msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
	if (msgid == -1)
	{
		if (errno != EEXIST)
		{
			printf("client msgget\n");
		}
		else
		{
			msgid = msgget(key, IPC_EXCL);
		}
	
	}

	return msgid;
}

int messageQueueSend(msgbuf_struct *msg)
{
	return msgsnd(msgid, msg, MSG_STRUCT_SIZE, 0);
}

ssize_t messageQueueRecv(msgbuf_struct *msg)
{
	return msgrcv(msgid, msg, MSG_STRUCT_SIZE, 1, 0);
}

int messageQueueUninit()
{
	return msgctl(msgid, IPC_RMID, NULL);
}
