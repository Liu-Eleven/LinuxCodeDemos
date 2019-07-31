
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>		
#include <sys/types.h>
#include <sys/stat.h>		

int main(int argc, char **argv)
{
	pid_t pid;
	char *message;
	int n;

	printf("fork program starting\n");
	pid = fork();
	if (pid > 0){
		int stat_val;
		pid_t child_pid;

		printf ("I am the parent of pid=%d!\n", pid);
		message = "I am the parent";
		n = 3;

		child_pid = wait(&stat_val);
		printf("child has finished: pid = %d\n", child_pid);
		if(WIFEXITED(stat_val)){
			printf("child exited with code %d\n", WEXITSTATUS(stat_val));
		}else{
			printf("child terminated abnormally\n");
		}
	}	
	else if (!pid){
		printf ("I am the child!\n");
		message = "I am the child";
		n = 5;
	}	
	else if (pid == -1){
		perror ("fork failed");
		exit(EXIT_FAILURE);
	}

	for(; n>0; n--){
		puts(message);
		sleep(1);
	}	

	return EXIT_SUCCESS;
}


