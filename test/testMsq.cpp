#include "../Messagequeue.h"
#include <stdio.h>

int main()
{
	FILE *fp = fopen("test.h264", "rb");
	int ret = initMsgQueue();
	if (ret < 0) {
		printf("create msq failed\n");
		return -1;
	}

	while (1) {
		usleep(1000 * 200);
        length = fread(sendBuffer, 1, 1024 * 10, fp);
        if (length <= 0) {

                break;
        }
        write(sockfd, (uint8_t *)&length, sizeof(uint64_t));

		write(sockfd, sendBuffer, length);
	}
    fclose(fp);
    close(sockfd);
	
}
