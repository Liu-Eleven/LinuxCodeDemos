
#include <stdlib.h>
#include <stdio.h>				
#include <string.h>		
#include <unistd.h>	

#include <sys/types.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg_com.h"

#define BUFFER_SIZE    (512)

int main(int argc, char **argv)
{
	int running = 1;
	int msgid;
	struct my_msg_st some_data;
	
	char buffer[BUFFER_SIZE] = {0};

	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);

	if(msgid == -1){
		fprintf(stderr, "msgget failed\n");
		exit(EXIT_FAILURE);
	}

	while(running){
		printf("enter some text:\n");
		fgets(buffer, TEXT_SZ, stdin);
		some_data.my_msg_type = 1;
		strncpy(some_data.some_text, buffer, BUFFER_SIZE);
		if(msgsnd(msgid, (void *)&some_data, TEXT_SZ, 0) == -1){
			fprintf(stderr, "msgsnd failed with error: %d\n", errno);
			exit(EXIT_FAILURE);
		}
		if(strncmp(some_data.some_text, "end", 3) == 0){
			running = 0;
		}
	}

	exit(EXIT_SUCCESS);

	return 0;
}

