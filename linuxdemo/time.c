
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>		
#include <sys/types.h>
#include <sys/stat.h>	
#include <time.h>	

// ./longopt --init -l --file=fred.c 'hi there'

#define _GNU_SOURCE
#include <getopt.h>

int main(int argc, char *argv[])
{
	int i;
	time_t the_time;

	for(i=1; i<10; i++){
		the_time = time((time_t *)0);
		printf("the time is %ld\n", the_time);
		sleep(2);
	}

	exit(0);
	
	return 0;
}

