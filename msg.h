#ifndef __MSG_H__
#define __MSG_H__

	#include <sys/msg.h>

	#define MSG_SIZE (1024 * 10)
	#define KEY_NAME "/home"

	#define RUNNING 1
	#define MSG_STRUCT_SIZE sizeof(Message)
		

	typedef struct _msgbuf 
	{
        	long m_type;     	/* message type, must be > 0 */
        	char m_text[MSG_SIZE];  /* message data */
    } Message;

	int messageQueueInit();
	int messageQueueSend(Message *msg);
	ssize_t messageQueueRecv(Message *msg);
	int messageQueueUninit();
#endif
