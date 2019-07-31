
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>	
#include <limits.h>		
#include <sys/types.h>
#include <sys/stat.h>		

// cat < /tmp/myfifo &
// echo "hello world" > /tmp/myfifo

#define FIFO_NAME    ("/tmp/myfifo")
#define BUFFER_SIZE    PIPE_BUF
#define TEN_MEG    (1024 * 1024 * 10)

int main(int argc, char **argv)
{
	int pipe_fd = 0;
	int res;
	int open_mode = O_WRONLY;
	int bytes_sent = 0;
	char buffer[BUFFER_SIZE + 1];

	if(access(FIFO_NAME, F_OK) == -1){
		res = mkfifo("/tmp/myfifo", 0777);
		if(res != 0){
			fprintf(stderr, "could not create fifo: %s\n", FIFO_NAME);
			exit(EXIT_FAILURE);
		}
	}
	
	printf("process %d opening fifo O_WRONLY\n", getpid());	
	pipe_fd = open(FIFO_NAME, open_mode);
	printf("process %d result %d\n", getpid(), pipe_fd);	

	if(pipe_fd != -1){
		while(bytes_sent < TEN_MEG){
			res = write(pipe_fd, buffer, BUFFER_SIZE);
			if(res == -1){
				fprintf(stderr, "write error on pipe\n");
				exit(EXIT_FAILURE);
			}
			bytes_sent += res;
		}
		close(pipe_fd);
	}else{
		exit(EXIT_FAILURE);
	}
	printf("process %d finished\n", getpid());
		
	exit(EXIT_SUCCESS);

	return 0;
}

