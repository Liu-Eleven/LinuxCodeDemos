#include<string.h>
#include<stdio.h>   //定义了标准库的文件流操作的函数, 也定义了如printf, perror等
#include<stdlib.h>  //定义了很多辅助性的函数, 其中包括exit
#include<unistd.h>  //定义了文件操作的函数, 如close
#include<sys/types.h>  //定义了一些u_short, u_int, pid_t之类的数据类型, 为了保持和旧系统的兼容性, socket编程非必要, 参见man socket, NOTE段 
#include<sys/socket.h> //定义bind, listen, accept等函数 
#include<netinet/in.h> //定义了ip地址的结构体
#include<arpa/inet.h> //定义了ip地址转换的函数, 如inet_pton, inet_ntop
#include<errno.h>
#include <time.h>

#define SERVER_PORT    (6666)
#define LISTENQ    (8)

int main(int argc, char** argv)
{
	int listenfd, connfd;
	socklen_t len;
	pid_t childpid;
	struct sockaddr_in servaddr, cliaddr;
	char buff[512];
	time_t ticks;
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);
	
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	printf("wait for client connect...\n");

	for(;;){
		len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);
		printf("connect from %s, port %d\n", 
			inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), 
			ntohs(cliaddr.sin_port));

		bzero(buff, sizeof(buff));
		int rlen = read(connfd, buff, sizeof(buff)-1);
		buff[rlen] = '\0';
		printf("receive: %s\n", buff);

		int wlen = write(connfd, buff, strlen(buff));
		printf("write finish. len = %d\n", wlen);

		close(connfd); 
	}

	return 0;
}