
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>		
#include <sys/types.h>
#include <sys/stat.h>		
#include <pthread.h>

// gcc -o thread thread.c -lpthread

void *thread_function(void *arg);

char message[] = "hello, world";

int main(int argc, char **argv)
{
	int res;
	pthread_t a_thread;
	void *thread_result;

	res = pthread_create(&a_thread, NULL, thread_function, (void *)message);
	if(res != 0){
		perror("thread create failed");
		exit(EXIT_FAILURE);
	}
	printf("waiting for thread to finish...\n");
	res = pthread_join(a_thread, &thread_result);
	if(res != 0){
		perror("thread join failed");
		exit(EXIT_FAILURE);
	}
	printf("thread joined, it returned: %s\n", (char *)thread_result);
	printf("message is now: %s\n", message);

	return 0;
}


void *thread_function(void *arg)
{
	printf("thread_function is running. argument was %s\n", (char *)arg);
	sleep(3);
	strcpy(message, "Bye!");
	pthread_exit("thank you for the cpu time");
}