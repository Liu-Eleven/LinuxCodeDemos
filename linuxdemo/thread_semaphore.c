
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>		
#include <sys/types.h>
#include <sys/stat.h>		
#include <pthread.h>
#include <semaphore.h>

// gcc -o thread_semaphore thread_semaphore.c -lpthread

void *thread_function(void *arg);
sem_t bin_sem;

#define WORK_SIZE    (1024)
char work_area[WORK_SIZE];

int main(int argc, char **argv)
{
	int res;
	pthread_t a_thread;
	void *thread_result;

	res = sem_init(&bin_sem, 0, 0);
	if(res != 0){
		perror("semaphore initialization failed");
		exit(EXIT_FAILURE);
	}

	res = pthread_create(&a_thread, NULL, thread_function, NULL);
	if(res != 0){
		perror("thread create failed");
		exit(EXIT_FAILURE);
	}

	printf("input some text. enter 'end' to finish\n");
	while(strncmp("end", work_area, 3) != 0){
		fgets(work_area, WORK_SIZE, stdin);
		sem_post(&bin_sem);
	}

	printf("\nwaiting for thread to finish...\n");
	res = pthread_join(a_thread, &thread_result);
	if(res != 0){
		perror("thread join failed");
		exit(EXIT_FAILURE);
	}
	printf("thread joined\n");
	sem_destroy(&bin_sem);
	exit(EXIT_SUCCESS);

	return 0;
}


void *thread_function(void *arg)
{
	sem_wait(&bin_sem);
	while(strncmp("end", work_area, 3) != 0){
		printf("you input %d characters\n", strlen(work_area) - 1);
		sem_wait(&bin_sem);
	}
	pthread_exit(NULL);
}