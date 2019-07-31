#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main (int args, char* arg[])
{
	int status;
	int number = 0;
	pid_t cpid;
	cpid = fork();
	if(cpid == 0) //child thread
	{
		char *argv[]={"sh", "doprinter.sh", "-dDEVICEWIDTHPOINTS=320", "-dDEVICEHEIGHTPOINTS=240", NULL};
		number = 5;
		printf("in child %d  number=%d\n",getpid(),number);
		int ret =execv("/bin/sh",argv);
		if(ret == -1)
		{
			printf("exit error");
			perror("exit");
			_exit(100);
		}
	}
	else //father thread
	{
		printf("in father %d  number=%d\n",getpid(),number);
		waitpid(cpid,&status,0);

	}
}
