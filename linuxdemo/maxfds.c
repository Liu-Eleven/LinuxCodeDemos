
#include <stdio.h>		/* for fprintf(), stderr, BUFSIZ */
#include <unistd.h>		/* for ssize_t */
#include <stdlib.h>

int
main(int argc, char **argv)
{
	printf("max fds: %d\n", getdtablesize());
	exit(0);
}
