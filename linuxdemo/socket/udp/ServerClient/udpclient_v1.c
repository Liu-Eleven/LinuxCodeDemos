#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024


int main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    int res, sfd;
    char buf[BUF_SIZE];
    
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) 
    {
        perror("socket error!\n");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    unsigned short port = 8080;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("192.168.100.168");

    if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) != 0)
    {
        perror("connect error");
	exit(1);
    }

    snprintf(buf, BUF_SIZE, "hello server, now i will send data to you");
    if(send(sfd, buf, strlen(buf), 0) <= 0)
    {
        perror("send failed.");
	exit(1);
    }
    
    printf("send data to clint: %s\n", buf);
    
    memset(buf, 0, sizeof(buf));

    res = recv(sfd, buf, BUF_SIZE, 0);   
    printf("recved %d bytes from server.\n", res);
    printf("reveive data from server : %s. \n", buf);
    
    return 0;
}
