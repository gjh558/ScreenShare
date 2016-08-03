#include "msg.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>

int msgid = -1;

void sig_catch_int(int signo)
{
	psignal(signo, "Catch signal");
	
	if (msgctl(msgid, IPC_RMID, NULL) == -1)
	{
		printf("server msgctl\n");
	}

	signal(SIGINT, sig_catch_int);
}


int main (int argc, char *argv[])
{
	msgid = messageQueueInit();
	
	msgbuf_struct msg_buf;
	msg_buf.m_type = 1;

	ssize_t rc = -1;
	while (RUNNING)
	{
		memset(msg_buf.m_text, '\0', MSG_SIZE);
		
		rc = messageQueueRecv(&msg_buf);
		if (rc == -1)
		{
			if (msgctl(msgid, IPC_RMID, NULL) == -1)
			{
				printf("server msgctl\n");
			}
			printf("server msgrcv\n");
		}

		msg_buf.m_text[MSG_SIZE - 1] = '\0';
		if (! strncasecmp(msg_buf.m_text, "quit"))
		{
			printf("server exit\n");
			if (msgctl(msgid, IPC_RMID, NULL) == -1)
			{
				printf("server msgctl");
			}
		}
		else
		{
			msg_buf.m_text[MSG_SIZE - 1] = '\0';
			printf("server-->> %s\n", msg_buf.m_text);
		}
	}//while (RUNNING)

	exit(EXIT_SUCCESS);
}
