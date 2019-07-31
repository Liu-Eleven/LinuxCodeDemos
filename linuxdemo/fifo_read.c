
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
	int open_mode = O_RDONLY;
	int bytes_read = 0;
	char buffer[BUFFER_SIZE + 1];

	memset(buffer, '\0', sizeof(buffer));

	printf("process %d opening fifo O_WRONLY\n", getpid());	
	pipe_fd = open(FIFO_NAME, open_mode);
	printf("process %d result %d\n", getpid(), pipe_fd);	

	if(pipe_fd != -1){
		do{
			res = read(pipe_fd, buffer, BUFFER_SIZE);
			bytes_read += res;
		}while(res > 0);
		close(pipe_fd);
	}else{
		exit(EXIT_FAILURE);
	}
	printf("process %d finished, %d bytes read\n", getpid(), bytes_read);
		
	exit(EXIT_SUCCESS);

	return 0;
}

