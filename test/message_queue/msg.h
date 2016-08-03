#ifndef __MSG_H__
#define __MSG_H__

	#include <sys/msg.h>

	#define MSG_SIZE (1024)
	#define KEY_NAME "/home"

	#define RUNNING 1
	#define MSG_STRUCT_SIZE sizeof(msgbuf_struct)
		

	typedef struct msgbuf 
	{
        	long m_type;     	/* message type, must be > 0 */
        	char m_text[MSG_SIZE];  /* message data */
    } msgbuf_struct;

	int messageQueueInit();
	int messageQueueSend(msgbuf_struct *msg);
	ssize_t messageQueueRecv(msgbuf_struct *msg);
	int messageQueueUninit();
#endif
