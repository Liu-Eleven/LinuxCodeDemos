
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
	int data_processed = 0;
	int file_pipes[2];
	const char some_data[] = "123";
	char buffer[BUFSIZ + 1];
	
	memset(buffer, '\0', sizeof(buffer));
	
	if(pipe(file_pipes) == 0){
		data_processed = write(file_pipes[1], some_data, strlen(some_data));
		printf("write %d bytes\n", data_processed);
		data_processed = read(file_pipes[0], buffer, BUFSIZ);
		printf("read %d bytes: %s\n", data_processed, buffer);
		exit(EXIT_SUCCESS);
	} 
	exit(EXIT_FAILURE);

	return 0;
}

