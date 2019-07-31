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


int main(int argc, char** argv)
{
	int cnt = 8;
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2){
		printf("usage: tcpclient <ipaddress>\n");
		exit(0);
	}
	do{ 
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(13);
		inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

		connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
		
		char recvline[512]={0};
		int n = 0;
		while((n = read(sockfd, recvline, 512)) > 0){
			printf("read: %s\n", recvline);
		}
		close(sockfd);
		
	}while(cnt--);
	exit(0);
} 