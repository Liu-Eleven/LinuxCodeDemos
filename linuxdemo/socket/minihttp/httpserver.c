#include <string.h>
#include <stdio.h>   
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <errno.h>
#include <time.h>
#include <ctype.h>

#define SERVER_PORT    (80)
#define LISTENQ    (8)


int get_line(int sock, char *buf, int size)
{
	int count = 0;
	char ch = '\0';
	int len = 0;

	while((count < size - 1) && ch != '\n')  {
		len = read(sock, &ch, 1);
		if(len == 1) {
			if(ch == '\r') {
				continue;
			} else if(ch == '\n') {
				buf[count] = '\0';
				break;
			}
			buf[count] = ch;
			count++;
		} else if(len == -1) {
			perror("read error.\n");
			break;
		} else {
			fprintf(stderr, "client close.\n");
			break;
		}
	}
	return count;
}


void do_http_request(int sock)
{
	int len = 0;
	char method[16] = {0};
	char url[256] = {0};
	char buf[512] = {0};

	// 读取客户端发送的http请求
	// 1.读取请求行
	len = get_line(sock, buf, sizeof(buf));
	if(len > 0) {
		int i = 0, j = 0;
		while(!isspace(buf[j]) && (i < sizeof(method) - 1)) {
			method[i] = buf[j];
			i++;
			j++;
		}
		method[i] = '\0';

		// 判断方法是否合法
		if(strncasecmp(method, "GET", i) == 0) { // GET 方法
			printf("request = %s\n", method);
			// 获取url	

			while(isspace(buf[++j])); // 跳过空格
			i = 0;
			while(!isspace(buf[j]) && (i < sizeof(url) - 1)) {
				url[i] = buf[j];
				i++;
				j++;
			}
			url[i] = '\0';
			printf("url = %s\n", url);
		} else {
			printf("other request = %s\n", method);
		}
	} else { // 出错处理

	}

	do {
		len = get_line(sock, buf, sizeof(buf));
		if(len > 0)
			printf("read line: %s\n", buf);
	} while(len > 0);
		
}


int main(int argc, char** argv)
{
	int listenfd = 0, connfd = 0;
	socklen_t len = 0;
	struct sockaddr_in servaddr, cliaddr;	
	char buf[512] = {0};

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
				inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)), 
				ntohs(cliaddr.sin_port));

		// 处理http请求
		do_http_request(connfd);

		//int wlen = write(connfd, buf, strlen(buf));
		//printf("write finish. len = %d\n", wlen);

		close(connfd); 
	}

	close(listenfd);

	return 0;
}