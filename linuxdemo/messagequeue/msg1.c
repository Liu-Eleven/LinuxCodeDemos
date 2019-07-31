
#include <stdlib.h>
#include <stdio.h>				
#include <string.h>		
#include <unistd.h>	

#include <sys/types.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg_com.h"



int main(int argc, char **argv)
{
	int running = 1;
	int msgid;
	struct my_msg_st some_data;
	long int msg_to_receive = 0;

	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);

	if(msgid == -1){
		fprintf(stderr, "msgget failed\n");
		exit(EXIT_FAILURE);
	}

	while(running){
		if(msgrcv(msgid, (void *)&some_data, TEXT_SZ, msg_to_receive, 0) == -1){
			fprintf(stderr, "msgrcv failed with error: %d\n", errno);
			exit(EXIT_FAILURE);
		}
		printf("you wrote: %s\n", some_data.some_text);
		if(strncmp(some_data.some_text, "end", 3) == 0){
			running = 0;
		}
	}

	if(msgctl(msgid, IPC_RMID, 0) == -1){
		fprintf(stderr, "msgctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

	return 0;
}

