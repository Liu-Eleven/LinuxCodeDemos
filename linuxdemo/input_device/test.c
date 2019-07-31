#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <linux/input.h>

// arm-linux-gnueabihf-gcc -static test.c -o test
// cat /proc/bus/input/devices 

int main(int argc, const char *argv[])
{
	int fd;
	struct input_event env;
	int c = 0;
	int i;
	
	fd =open("/dev/input/event0", 0666);
	
	if(fd<=0) {
		puts("open error");
		return -1;
	}
	
	while(1) {
		
		c=read(fd,&env,sizeof(struct input_event));
		if(c<0) {
			perror("read error");
			return -1;
		}
		
		printf("type:%d code:%d value:%d\n", env.type, env.code, env.value);
	}
	return 0;
}

