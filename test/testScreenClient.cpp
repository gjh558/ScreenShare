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
    FILE *fp = fopen("recv1.h264", "w+");
	int type_side = 2;
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

	char recvLine[1024 * 10];

    write(sockfd, sendline, 12);

	while (1) {
		int len = read(sockfd, recvLine , 1024 * 10);
		printf("recv : %s\n", recvLine);
        fwrite(recvLine, 1, len, fp);
	}
    fclose(fp);
    close(sockfd);

    return 1;
}
