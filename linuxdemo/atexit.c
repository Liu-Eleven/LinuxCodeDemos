
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void out(void)
{
	printf("atexit( ) succeeded!\n");
}

void callback1(void) { printf("callback1 called\n"); }
void callback2(void) { printf("callback2 called\n"); }
void callback3(void) { printf("callback3 called\n"); }

int main(int argc, char **argv)
{
	if (atexit(out))
		fprintf(stderr, "atexit( ) failed!\n");
	printf("registering callback1\n");
	atexit(callback1);
	printf("registering callback2\n");
	atexit(callback2);
	printf("registering callback3\n");
	atexit(callback3);

	printf("exiting now\n");
	exit(0);
	return 0;
}
