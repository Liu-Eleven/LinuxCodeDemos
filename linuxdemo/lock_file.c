
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>		
#include <sys/types.h>
#include <sys/stat.h>		

/*
how to run
rm -f /tmp/lck.test
./lock_file & ./lock_file
*/
const char *lock_file = "/tmp/lck.test";

int main(int argc, char **argv)
{
	int file_desc;
	int tries = 10;

	while(tries--){
		file_desc = open(lock_file, O_RDWR | O_CREAT | O_EXCL, 0444);
		if(file_desc == -1){
			printf("%d - lock already present\n", getpid());
			sleep(3);
		}else{
			printf("%d - i have exclusive access\n", getpid());
			sleep(1);
			close(file_desc);
			unlink(lock_file);
			sleep(2);
		}
	}
	exit(EXIT_SUCCESS);
	return 0;
}


