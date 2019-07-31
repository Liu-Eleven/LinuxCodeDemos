
#include <stdlib.h>
#include <stdio.h>				
#include <string.h>		
#include <unistd.h>	

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm_com.h"



int main(int argc, char **argv)
{
	int running = 1;
	void *shared_memory = (void *)0;
	struct share_use_st *share_stuff;
	int shmid;
	
	srand((unsigned int)getpid());

	shmid = shmget((key_t)1234, sizeof(struct share_use_st), 0666 | IPC_CREAT);

	if(shmid == -1){
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shared_memory = shmat(shmid, (void *)0, 0);
	if(shared_memory == (void *)-1){
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	share_stuff = (struct share_use_st *)shared_memory;
	share_stuff->written_by_you = 0;
	while(running){
		if(share_stuff->written_by_you){
			printf("you wrote: %s\n", share_stuff->some_text);
			sleep(rand() % 4);
			share_stuff->written_by_you = 0;
			if(strncmp(share_stuff->some_text, "end", 3) == 0){
				running = 0;
			}
		}
	}

	if(shmdt(shared_memory) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	
	if(shmctl(shmid, IPC_RMID, 0) == -1){
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

	return 0;
}

