#include <stdio.h>    
#include <stdlib.h>   
#include <string.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "cencode.h"
#include "cdecode.h"
#include "DefineCmd.h"
#include "camera.h"
#include "dragonboard_inc.h"


#define LENGTH_OF_LISTEN_QUEUE    (32)
#define BUFFER_SIZE    (1024)

unsigned char buffer[BUFFER_SIZE];
//pthread_mutex_t mutex;


///////////////////////////////////////////



void get_args(unsigned char *buf, unsigned int bufsize, tArg *arg)
{
	int i;
	int offset = 0;
	int start = offset;
	int end = offset;
	
	memset((void *)arg, 0, sizeof(tArg));
	
	while(offset <= bufsize)
	{
		if(buf[offset] == ',')
		{
			end = offset - 1;
			for(i=start; i<=end; i++)
			{
				arg->argv[arg->argc][i-start] = buf[i];
			}
			arg->argc++;
			start = offset + 1;
		}
		offset++;
	}
	
	for(i=start; i<=bufsize; i++)
	{
		arg->argv[arg->argc][i-start] = buf[i];
	}
	arg->argc++;

#if 0
	printf("argc = %d\n", arg->argc);
	for(i=0; i<arg->argc; i++)
	{
		printf("arg%d = %s\n", i, arg->argv[i]);
	}
#endif
}




char ByteToAsciiChar(unsigned char b)
{
	if(b < 10)
		b += 0x30;
	else
		b = b + 0x41-0x0A;
	return b;
}

void do_command_operation(int fd, unsigned char *buf)
{
	db_msg("do_command_operation()\n");




}





/*
* server send picture to client
*/

tArg tArgs;

/*
* set up a destination buffer large enough to hold the encoded data
*/
char soutput[128*1024]; 
char sendbuf[128*1024];
char sfilebuf[128*1024];

void do_send_file(int fd, unsigned char *buf)
{
	db_msg("do_send_file()\n");
	
	memset(soutput, 0 ,sizeof(soutput));
	memset(sendbuf, 0 ,sizeof(sendbuf));

//	pthread_mutex_lock (&mutex);
	do_camera_capture();
//	pthread_mutex_unlock(&mutex);
	
	{
		int fileSize = 0;
		int offset = 0;
		int total_cnt = 0;

		int filefd = open("./picture.jpg", O_RDONLY);

		if(filefd == -1)
		{
			db_error("file:%s not found\n", "./picture.jpg");
			return;
		}
		
		fileSize = lseek(filefd, 0, SEEK_END);
		db_msg("file size:%d \n", fileSize);
		lseek(filefd, 0, SEEK_SET);

		bzero(sfilebuf, sizeof(sfilebuf));
			
		read(filefd, sfilebuf, fileSize);
	  	close(filefd);


		/*
		* keep track of our encoded position 
		*/
		char* c = soutput;
		/*
		* store the number of bytes encoded by a single call
		*/
		int cnt = 0;
		/*
		* we need an encoder state
		*/
		base64_encodestate s;
	
		/*---------- START ENCODING ----------*/
		/*
		* initialise the encoder state
		*/ 
		base64_init_encodestate(&s);
		/*
		* gather data from the input and send it to the output
		*/
		cnt = base64_encode_block(sfilebuf, fileSize, c, &s);
		c += cnt;
		total_cnt += cnt;

		/*
		* since we have encoded the entire input string, we know that 
		* there is no more input data; finalise the encoding 
		*/
		cnt = base64_encode_blockend(c, &s);
		c += cnt;
		total_cnt += cnt;

		/*---------- STOP ENCODING  ----------*/
	
		/*
		*  we want to print the encoded data, so null-terminate it: 
		*/
		*c = 0;
		total_cnt++;
		
			
		int i = 0;

		memset(buffer, 0 ,sizeof(buffer));
		sprintf(buffer, "%d", total_cnt);
		db_msg("encode size:%ld \n", total_cnt);

		int last = 0;
		for (i = sizeof(buffer)-1; i>=0 ; i--)
		{
			if(buffer[i] != 0)
			{
				last = i;
				break;
			}
			
		}
		
		offset = 0;
		for (i = 0; i<=last ; i++)
		{
			sendbuf[offset++] = buffer[i];		
		}
		
		sendbuf[offset++] = ',';
		
		for (i = 0; i<=total_cnt ; i++)
		{
			sendbuf[offset++] = soutput[i];		
		}

		/*
		* send picture datas
		*/
		int sendlength = 0;
		if((sendlength = write(fd, sendbuf, offset)) < 0)
		{
			db_error("file send length:%d bytes fail.\n", sendlength);
			return;		
		}
		db_msg("file send length:%d bytes.\n", sendlength);
	
		db_msg("File Transfer Successful!\n");

	}

}



tArg rArgs;
//char routput[128*1024]; 
//char rfilebuf[128*1024];
void do_receive_file(int fd, unsigned char *buf)
{
	db_msg("do_receive_file()\n");

	/*
	* open file and write datas
	*/
	int filefd = open("printfile.png", O_WRONLY | O_CREAT);
	if(filefd == -1)
	{
		db_error("File:\t%s Can Not Open To Write\n", "printfile.png");
		exit(1);
	}

	get_args(buf, 512, &rArgs);

	int fileSize = atoi(rArgs.argv[2]);
	db_msg("file size:%ld \n", fileSize);

	unsigned char *filebuf = malloc(fileSize);
	//unsigned char *filebuf = rfilebuf;
	bzero(buffer, sizeof(buffer));
 
	int rlength = 0; 
	int wlength = 0;
	int off = 0;
	int rcnt = 0;
	while(rlength = recv(fd, buffer, fileSize, 0))  
	{  

		if (rlength <= 0)  
		{  
			db_error("recieve data from client failed!\n");  
			break;  
		}  
	  	
		for(off=0; off<rlength; off++)
		{
			filebuf[wlength++] = buffer[off];			
		}
 
		bzero(buffer, sizeof(buffer));  
		rcnt+=rlength;
		db_msg("read length: %d  receive size:%d \n", rlength, rcnt);
		if(rcnt >= fileSize)
		{
			db_msg("receive size:%d \n", rcnt);
			break;
		}
	}  

	db_msg("file receive length:%d\n", wlength);

	/*
	* set up a destination buffer large enough to hold the encoded data
	*/
	char* output = (char*)malloc(fileSize*2);
	//char* output = (char*)routput;
	
	/*
	* keep track of our decoded position
	*/
	char* c = output;
	
	/*
	* store the number of bytes decoded by a single call
	*/
	int cnt = 0;
	/*
	* we need a decoder state
	*/
	base64_decodestate s;
	
	/*---------- START DECODING ----------*/
	/*
	* initialise the decoder state
	*/
	base64_init_decodestate(&s);
	/*
	* decode the input data
	*/
	cnt = base64_decode_block(filebuf, fileSize, c, &s);
	c += cnt;
	/* note: there is no base64_decode_blockend! */
	/*---------- STOP DECODING  ----------*/
	
	/*
	* we want to print the decoded data, so null-terminate it: 
	*/
	*c = 0;
	db_msg("decode size:%ld \n", cnt);
	
	if(write(filefd, output, cnt) == -1)
	{
		db_error("File:\t%s Write Failed\n", "printfile.png");
	}

	free(output);
	free(filebuf);
	close(filefd);

	db_msg("Receive File:\t%s From client IP Successful!\n", "printfile.png");

}



/*
* thread for do protocol logic
*/
void* do_connect_thread(void* arg)
{
	int size;
	int *parg = (int *)arg;
	int fd = *parg;
	db_msg("create socket handle thread fd=%d\n", fd);

	unsigned char *buf = (unsigned char *)malloc(1024);
	
	/*
	* first read command and status and file size
	* must include description in first package 
	*/
	while((size = read(fd, buf, 512))>0)
	{	
		db_msg("read size: %d\n", size);
		db_msg("command type: %c\n", buf[0]);
		db_msg("commnication status: %c\n", buf[2]);
		
		switch(buf[0])
		{
			case COMMAND_OPERATION: // command such as printer
				//do_command_operation(fd, buf); 
				break;
			case SEND_FILE: // send file from server to client
				do_send_file(fd, buf);
				break;
			case RECEIVE_FILE: // send file from client to server
				do_receive_file(fd, buf); 
				break;
		}

	}
	
	close(fd);
	free(buf);
	
	db_msg("socket handle thread out\n");
	
	return 0;
}



int main(int argc, char *argv[])
{
	int listen_fd;
	int com_fd;
	int ret;
	int len;
	int port;
	pthread_t tid;
	struct sockaddr_in clt_addr;
	struct sockaddr_in srv_addr;

	/*
	* check parameter
	*/ 
	if(argc!= 2)
	{
		db_error("Usage:%s port\n",argv[0]);
		return 1;
	}

	/*
	* get listen port
	*/
	port = atoi(argv[1]);

	/*
	* create socket 
	*/
	listen_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0)
	{
		db_error("cannot create listening socket");
		return 1;
	}

	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(port);

	ret = bind(listen_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if(ret == -1)
	{
		db_error("cannot bind server socket");
		close(listen_fd);
		return 1;
	}


	ret = listen(listen_fd, LENGTH_OF_LISTEN_QUEUE);
	if(ret == -1)
	{
		db_error("cannot listen the client connect request");
		close(listen_fd);
		return 1;
	}

	do_camera_capture_init();
//	pthread_mutex_init (&mutex,NULL);  

	while(1)
	{
		len = sizeof(clt_addr);
		com_fd = accept(listen_fd, (struct sockaddr *)&clt_addr, &len);
		db_msg("accept socket connect fd=%d\n", com_fd);
		
		if(com_fd < 0)
		{
			if(errno == EINTR)
			{
			    continue;
			}
			else
			{
				db_error("cannot accept client connect request\n");
				close(listen_fd);
				return 1;
			}
		}

		if((pthread_create(&tid, NULL, do_connect_thread, &com_fd)) == -1)
		{
			db_error("pthread_create error\n");
			close(listen_fd);
			close(com_fd);
			return 1;
		}
	}

	return 0;
}
