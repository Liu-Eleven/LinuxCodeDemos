/* datagram_client.c:
 *
 * Example datagram server:
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
                    

static void usage(const char* proc)
{ 
    printf("Usage: %s [locaal_ip] [local_port]\n", proc);
	printf("Usage: %s 127.0.0.1 9090\n", proc);
    exit(1);
}

int main(int argc, char *argv[])   
{                           
    if (argc != 3)      
    { 
        usage(argv[0]);         
        return 1;                                               
    }           

    int sock = socket(AF_INET, SOCK_DGRAM, 0);//IPV4  SOCK_DGRAM 数据报套接字（UDP协议）  
    if(sock < 0)
    { 
        perror("socket\n");
        return 2;
    }

    struct sockaddr_in server_addr;   
    server_addr.sin_family = AF_INET;   
    server_addr.sin_port = htons(atoi(argv[2]));          
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    socklen_t len = sizeof(server_addr);  
    char buf[1024];
    char* msg = "hello world";
	int cnt = 6;
    while(cnt)
    {   
		printf("send: %s\n", msg);
        if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&server_addr, len) < 0 )
        { 
            perror("send:");  
            exit(3);   
        }  
        struct sockaddr_in tmp; 
        len = sizeof(tmp);
        int ret = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&tmp ,&len);  
        if(ret > 0) 
        { 
            buf[ret] = 0;   
            printf("receive: %s\n", buf);
            //break;
        }
		
		cnt--;
    }
    close(sock);   
    return 0;   
}  

