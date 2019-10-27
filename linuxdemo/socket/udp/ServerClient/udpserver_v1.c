#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>


#define BUF_SIZE 1024


int main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    struct sockaddr_in client_addr;
    int res, sfd, client_addr_len;
    char buf[BUF_SIZE];
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    unsigned short port = 8080;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) 
    {
        perror("socket error!\n");
        exit(1);
    }
    
    res = bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (res == -1) 
    {
        perror("bind error!\n");
        exit(1);
    }
    
    while (1) 
    {
        printf("waiting for client ...\n");
        
        res = recv(sfd, buf, BUF_SIZE, 0);
        if (res == -1) 
	{
            perror("recvfrom error\n");
            exit(1);
        }
        printf("revceived data : %s\n", buf);
           
        snprintf(buf, BUF_SIZE, "hello client! Total reveive : %d.", res);
//        res = sendto(sfd, buf, strlen(buf), 0, (struct sockaddr*)&client_addr, client_addr_len);
        res = send(sfd, buf, strlen(buf), 0);
	if (res != strlen(buf) ) 
	{
            perror("sendto error\n");
            exit(1);
        }
        
        printf("send %d bytes to peer", res);

        printf("send data < %s > to client\n\n", buf);
    }
}
