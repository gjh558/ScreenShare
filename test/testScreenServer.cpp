#include <stdio.h>  
#include <strings.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
//#include <linux/in.h>  
#include <stdlib.h>  
#include <memory.h>  
#include <arpa/inet.h> 
#include <netinet/in.h>

int main(int argc, char **argv)
{
    FILE *fp = fopen("test.h264", "rb");

	int type_side = 1;
	int id = 1;
	int classid = 2;

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12345);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	int ss[3] = {type_side, id, classid};

    char sendline[100];
    memcpy(sendline, (char *)ss, sizeof(ss));

    write(sockfd, sendline, 12);

    char sendBuffer[1024 * 10];
    uint64_t length ;

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

    return 1;
}
