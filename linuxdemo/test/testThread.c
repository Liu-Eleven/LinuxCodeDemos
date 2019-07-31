#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
* thread for do protocol logic
*/
void *thr_fn(void *arg)
{
	printf("arg: %s\n", (char *)arg);

	//while((size=read(fd, buf, 512))>0)
	{

	}
	printf("thread handle out....\n");
	return 0;
}


int main (int args, char* arg[])
{
	pthread_t tid;
	const char* line="cbcbcbcb";
	if((pthread_create(&tid, NULL, thr_fn, line)) == -1)
	{
		perror("pthread_create error");
		return 1;
	}
	while(1);
	return 0;
}
