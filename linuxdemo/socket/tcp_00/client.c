
#include <netinet/in.h>
#include <sys/types.h>    
#include <sys/socket.h>    
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>      
#include <time.h>                
#include <arpa/inet.h>
#include "crc.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

///////////////////////////////////////////////////////////////


#define SERVER_PORT 6666
#define BUFFER_SIZE 2048

#define CMD_RECEIVE    (1)
#define CMD_SEND    (2)
#define CMD_FILE_DATAS    (3)
#define CMD_COMPLETE    (4)

///////////////////////////////////////////////////////////////

char buffer[BUFFER_SIZE];
char filebuf[BUFFER_SIZE];

char file_name[256];
char local_file_name[256];
int file_name_length = 0;
	
///////////////////////////////////////////////////////////////

/*
* receive file from server
*/
void receive_file(int fd)
{
	int i = 0;
	int pos = 0;
	int length = 0;
	int ret = -1;
	unsigned short crc = 0;
	
	printf("receive_file(%d)\n", fd);
	
    bzero(buffer, BUFFER_SIZE);
	pos = 0;
	// 0-1 length    2-3 command    4-5 crc16
	buffer[pos++] = 6;
	buffer[pos++] = 0;
	buffer[pos++] = CMD_SEND;
	buffer[pos++] = 0;
	crc = crc16(buffer, pos);
    RecoverSelfData((unsigned char*)&crc, 2);
	buffer[pos++] = crc;
	buffer[pos++] = crc >> 8;
	
	//print_hex(buffer, 6);
	if(send(fd, buffer, pos, 0) < 0)
	{
		printf("send command error\n");
	}

		
	//////////////////////////////////////////////	
	
	// send file name
	bzero(buffer, BUFFER_SIZE);
	pos = 0;
	file_name_length += 2;
	buffer[pos++] = file_name_length;
	buffer[pos++] = file_name_length >> 8;
	
	for(i=0; i<file_name_length - 2; i++)
	{
		buffer[pos++] = file_name[i]; 
	}
	
	crc = crc16(buffer, pos);
    RecoverSelfData((unsigned char*)&crc, 2);
	buffer[pos++] = crc;
	buffer[pos++] = crc >> 8;
	
	//print_hex(buffer, pos);
	if(send(fd, buffer, pos, 0) < 0)
	{
		printf("send name error\n");
	}
	
	//////////////////////////////////////////////
	
	// receive file datas 
	// 0-1 length    2-n file datas    n+1-n+2 crc16
	printf("open %s \n", local_file_name);
	int filefd = open(local_file_name,  O_WRONLY | O_CREAT);

	if(filefd == -1)
	{
		printf("file:%s not found\n", local_file_name);
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

	bzero(buffer, BUFFER_SIZE);
	pos = 0;

	close(fd);
	
}

/*
* send file to server
*/
void send_file(int fd)
{
	int i = 0;
	int pos = 0;
	unsigned short crc = 0;
	
	printf("send_file(%d)\n", fd);
	
    bzero(buffer, BUFFER_SIZE);
	pos = 0;
	// 0-1 length    2-3 command    4-5 crc16
	buffer[pos++] = 6;
	buffer[pos++] = 0;
	buffer[pos++] = CMD_RECEIVE;
	buffer[pos++] = 0;
	crc = crc16(buffer, pos);
    RecoverSelfData((unsigned char*)&crc, 2);
	buffer[pos++] = crc;
	buffer[pos++] = crc >> 8;
	
	//print_hex(buffer, 6);
	if(send(fd, buffer, pos, 0) < 0)
	{
		printf("send command error\n");
	}

		
	//////////////////////////////////////////////	
	
	// send file name
	bzero(buffer, BUFFER_SIZE);
	pos = 0;
	file_name_length += 2;
	buffer[pos++] = file_name_length;
	buffer[pos++] = file_name_length >> 8;
	
	for(i=0; i<file_name_length - 2; i++)
	{
		buffer[pos++] = file_name[i]; 
	}
	
	crc = crc16(buffer, pos);
    RecoverSelfData((unsigned char*)&crc, 2);
	buffer[pos++] = crc;
	buffer[pos++] = crc >> 8;
	
	//print_hex(buffer, pos);
	if(send(fd, buffer, pos, 0) < 0)
	{
		printf("send name error\n");
	}
	
	//////////////////////////////////////////////
	
	// send file datas
	printf("open %s \n", local_file_name);
	int filefd = open(local_file_name, O_RDONLY);

	if(filefd == -1)
	{
		printf("file:%s not found\n", local_file_name);
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


int main(int argc,char **argv)
{
	int i = 0;
	if(argc < 3)
	{                     
		printf("%s ip rcv/snd server_file_name client_file_name\n", argv[0]);
		exit(1);
	}
	
	if(argc == 5)
	{
		i = 0;
		bzero(file_name, 256);
		char *argv2 = argv[3]; 
		for(i=0; i<256; i++)
		{
			file_name_length++;
			if(argv2[i])
			{
				file_name[i] = argv2[i];
				printf("%c", file_name[i]);
			}			
			else
			{
				break;
			}
		}
		printf("\n");
		
		bzero(local_file_name, 256);
		strcpy(local_file_name, argv[4]);		
	}

	struct sockaddr_in server_addr;
	
	bzero(&server_addr, sizeof(server_addr)); //把一段内存区的内容全部设置为0
	server_addr.sin_family = AF_INET;    //internet协议族
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(SERVER_PORT);   
	
	int sfd;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd < 0)
    {
		printf("socket error\n");
		exit(0);
    }
 
	if(connect(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Can Not Connect To %s\n", argv[1]);
        exit(1);
    }

	// receive file from server
	if (strcmp(argv[2], "rcv") == 0) 
	{
		receive_file(sfd);
	}
	
	// send file to server
	if (strcmp(argv[2], "snd") == 0) 
	{
		send_file(sfd);
	}

	return 0;
}