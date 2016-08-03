#include "msg.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

int msgid = -1;

void sig_catch_int(int signo)
{
	psignal(signo, "Catch signal");
	
	if (msgctl(msgid, IPC_RMID, NULL) == -1)
	{
		printf("client msgctl\n");
	}

	signal(SIGINT, sig_catch_int);
}


int main (int argc, char *argv[])
{
	msgid = messageQueueInit();
	
	msgbuf_struct msg_buf;
	msg_buf.m_type = 1;

	size_t wc = -1;
	while (RUNNING)
	{
		memset(msg_buf.m_text, '\0', MSG_SIZE);
		
		fprintf(stderr, "<<--client \n");
		fgets(msg_buf.m_text, MSG_SIZE, stdin);
		
		wc = messageQueueSend(&msg_buf);
		if (wc == -1)
		{
			if (msgctl(msgid, IPC_RMID, NULL) == -1)
			{
				printf("client msgctl\n");
			}
			printf("client msgsnd");
		}

		if (! strncasecmp(msg_buf.m_text, "quit"))
		{
			printf("client exit\n");
			break;
		}
	}//while (RUNNING)

	exit(EXIT_SUCCESS);
}
