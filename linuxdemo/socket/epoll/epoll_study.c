#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_EVENTS 128
#define PORT 9000
#define MAX_BUFFER 1024

int main(int argc, char *argv[])
{
	//初始化
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;	
	struct epoll_event ev, events[MAX_EVENTS] = {0};
	int listen_fd, client_fd;
	socklen_t clientlen;
	int port = PORT;
	int epoll_fd;
	int epoll_nfds;
	int i, j;
	char data[MAX_BUFFER];
	
	if(argc == 2 && isdigit(argv[1]))
	{
		port = atoi(argv[1]);
	}
	
	memset(&serveraddr, 0, sizeof(serveraddr));
	memset(&clientaddr, 0, sizeof(clientaddr));	
	//创建监听SOCKET并绑定
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0)
	{
		fprintf(stderr, "创建监听SOCKET失败");
		return -1;
	}
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_aton("127.0.0.1", &(serveraddr.sin_addr));
	
	bind(listen_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	listen(listen_fd, 10);	
	
	//创建EPOLL
	epoll_fd = epoll_create(MAX_EVENTS);
	
	//控制EPOLL
	ev.data.fd = listen_fd;
	ev.events = EPOLLIN|EPOLLET;
	
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);
	
	//处理EPOLL
	
	while(1)
	{
		epoll_nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 500);
		for(i=0; i<epoll_nfds; i++)
		{
			if(events[i].data.fd == listen_fd)
			{
				printf("new connection\r\n");
				client_fd = accept(listen_fd, (struct sockaddr *)&clientaddr, &clientlen);
				if(client_fd < 0)
			  {
			  	perror("client_fd < 0");
			  	exit(1);
			  } 
			  printf("New connection from %s\n", inet_ntoa(clientaddr.sin_addr));
			  ev.data.fd = client_fd;
			  ev.events = EPOLLIN|EPOLLET;
			  printf("add connection\r\n");
			  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);			
			}
			else if(events[i].events&EPOLLIN)
			{
				fprintf(stdout, "EPOLLIN\n");
				client_fd = events[i].data.fd;
				if(client_fd < 0)
				{
					continue;
				}
				
				memset(data, 0, sizeof(data));
				if(j = read(client_fd, data, MAX_BUFFER) < 0)
				{
					if(errno == ECONNRESET)
					{
						close(client_fd);
						events[i].data.fd = -1;
					}
				}
				fprintf(stdout, "Read Data %s\n",data);
				
				ev.data.fd = client_fd;
				ev.events = EPOLLOUT|EPOLLET;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev); 
			}
			else if(events[i].events&EPOLLOUT)
			{
				client_fd = events[i].data.fd;
				write(client_fd, data, j);
				ev.data.fd = client_fd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
			}			
		}		
	}
	
	
	//清理环境	
	close(epoll_fd);
	close(listen_fd);
	
	return 0;
}