CC := gcc
CFLAGS += -w -g -O -O2 -O3 -o
RM := rm -rf *~

all:
	$(CC) $(CFLAGS) msg_server msg_server.c msg.c
	$(CC) $(CFLAGS) msg_client msg_client.c msg.c
clean:
	$(RM)
