
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "crc.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

///////////////////////////////////////////////////////////////

#define SERVER_PORT 6666
#define LISTEN_QUEUE 20
#define BUFFER_SIZE 2048

#define CMD_RECEIVE    (1)
#define CMD_SEND    (2)
#define CMD_FILE_DATAS    (3)
#define CMD_COMPLETE    (4)

////////////////////////////////////////////////////////////////

char buffer[BUFFER_SIZE];
char filebuf[BUFFER_SIZE];
int flag_exit = 0;

////////////////////////////////////////////////////////////////

/*
* receive file from client
*/
void receive_file(int fd)
{
	int i = 0;
	int ret = -1;
	int length = 0;
	char file_name[256];

	//printf("receive_file(%d)\n", fd);
	
	bzero(buffer, BUFFER_SIZE);
	bzero(file_name, 256);
	
	// receive file name 
	// 0-1 length    2-n file name data    n+1-n+2 crc16
	recv(fd, buffer, 2, 0);
	length = buffer[1]<<8 | buffer[0];		
	recv(fd, (char *)&buffer[2], length, 0);
	ret = package_check(buffer, length + 2);
	if(ret)
	{
		for(i=0; i<255; i++)
		{
			file_name[i] = buffer[2+i];
		}
		bzero(buffer, BUFFER_SIZE);
		printf("file name:%s\n",file_name);
	}
	else
	{
		printf("crc error!\n");
		return;
	}
	
	// receive file datas 
	// 0-1 length    2-n file datas    n+1-n+2 crc16
	printf("open %s \n", file_name);
	int filefd = open(file_name,  O_WRONLY | O_CREAT);

	if(filefd == -1)
	{
		printf("file:%s not found\n", file_name);
	}
	else
	{
		printf("start receive file...\n");
		int flag = 1;
		
		while(flag)
		{
			bzero(buffer, BUFFER_SIZE);
			bzero(filebuf, BUFFER_SIZE); 
	
			recv(fd, buffer, 4, 0);
			length = buffer[1]<<8 | buffer[0];
			//printf("length = %d\n", length);	
			if(buffer[2] == CMD_COMPLETE)
			{
				flag = 0;
				flag_exit = 1;
				printf("completed! stop transfer.\n");
				break;
			}	
			
			recv(fd, (char *)&buffer[4], length - 2, 0);		
			ret = package_check(buffer, length + 2);
			if(ret)
			{
				for(i=0; i<length - 4; i++)
				{
					filebuf[i] = buffer[4+i];
				}
				if(write(filefd, filebuf, length - 4) == -1)
				{
					printf("file:\t%s write failed\n", file_name);
				}
			}
			else
			{
				printf("crc error!\n");
				return;
			}
	
		}

		close(filefd);		
	}
	
	close(fd);
}

/*
* send file to client
*/
void send_file(int fd)
{
	int pos = 0;
	int i = 0;
	int ret = -1;
	int length = 0;
	char file_name[256];
	unsigned short crc = 0;
	
	//printf("send_file(%d)\n", fd);
	
	bzero(buffer, BUFFER_SIZE);
	bzero(file_name, 256);
	
	// receive file name 
	// 0-1 length    2-n file name data    n+1-n+2 crc16
	recv(fd, buffer, 2, 0);
	length = buffer[1]<<8 | buffer[0];		
	recv(fd, (char *)&buffer[2], length, 0);
	ret = package_check(buffer, length + 2);
	if(ret)
	{
		for(i=0; i<255; i++)
		{
			file_name[i] = buffer[2+i];
		}
		bzero(buffer, BUFFER_SIZE);
		printf("file name:%s\n",file_name);
	}
	else
	{
		printf("crc error!\n");
		return;
	}
	
	// send file datas 
	// 0-1 length    2-n file datas    n+1-n+2 crc16
	// send file datas
	printf("open %s \n", file_name);
	int filefd = open(file_name, O_RDONLY);

	if(filefd == -1)
	{
		printf("file:%s not found\n", file_name);
	}
	else
	{
		printf("start send file...\n");
		
		int len=0;
		bzero(buffer, BUFFER_SIZE);
		bzero(filebuf, BUFFER_SIZE); 
		pos = 0;
		while((len = read(filefd, filebuf, 1024))>0)
		{
			buffer[pos++] = (len + 4);
			buffer[pos++] = (len + 4) >> 8;
			
			buffer[pos++] = CMD_FILE_DATAS;
			buffer[pos++] = 0;
			
			for(i=0; i<len; i++)
			{
				buffer[pos++] = filebuf[i]; 
			}
			
			crc = crc16(buffer, pos);
			RecoverSelfData((unsigned char*)&crc, 2);
			buffer[pos++] = crc;
			buffer[pos++] = crc >> 8;

			if(send(fd, buffer, pos, 0) < 0)
			{
				printf("send file datas error\n");
			}
			
			bzero(buffer, BUFFER_SIZE);
			bzero(filebuf, BUFFER_SIZE); 
			pos = 0;
		}
		
		close(filefd);		
	}
	
	//////////////////////////////////////////////
	
	// send complete command 		
	bzero(buffer, BUFFER_SIZE);
	pos = 0;
	
	buffer[pos++] = 6;
	buffer[pos++] = 0;			
	buffer[pos++] = CMD_COMPLETE;
	buffer[pos++] = 0;
	crc = crc16(buffer, pos);
	RecoverSelfData((unsigned char*)&crc, 2);
	buffer[pos++] = crc;
	buffer[pos++] = crc >> 8;

	//print_hex(buffer, pos);
	if(send(fd, buffer, pos, 0) < 0)
	{
		printf("send complete command error\n");
	}
			
	bzero(buffer, BUFFER_SIZE);
	pos = 0;

	close(fd);
}

void child_handler(int fd)
{
	int ret = -1;
	
	bzero(buffer, BUFFER_SIZE);
	
	// 0-1 length    2-3 command    4-5 crc16
	recv(fd, buffer, 6, 0);
	//print_hex(buffer, 6);
	
	ret = package_check(buffer, 6);
	if(ret == 0)
	{
		printf("crc error!\n");
		return;
	}
	
	int cmd = buffer[3]<<8 | buffer[2];
	switch(cmd)
	{
		case CMD_RECEIVE:
		{
			receive_file(fd);
		}
		break;
		case CMD_SEND:
		{
			send_file(fd);
		}
		break;
		default:
		{
			printf("receive command error! %d\n", cmd);
		}
		break;
	}				
}

int main(int argc,char **argv)
{
	struct sockaddr_in server_addr;
	
	bzero(&server_addr, sizeof(server_addr));//全部置零
	
	//设置地址相关的属性
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);


	//创建套接字
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(server_socket < 0) 
	 {
		 printf("socket create error\n");
		 exit(1);
	 }
	 
	//绑定端口
	if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		printf("bind error\n");
		exit(1);
	}
	
	//服务器端监听
	if(listen(server_socket, LISTEN_QUEUE))
	{
		printf("Server listen error\n");
		exit(1);
	}

	//服务器端一直运行
	while(1)
	{
		pid_t pid;
		struct sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);
		 
		//accept返回一个新的套接字与客户端进行通信
		int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
		 
		if(new_server_socket == -1)
		{
			printf("accept error\n");
			continue;
		}
		else
		{	
			pid = fork();
			if(pid == 0)
			{
				child_handler(new_server_socket);
			}			
			else
			{
				close(new_server_socket);
			}
		}
	
		close(new_server_socket);
	}
	
	return 0;
}

