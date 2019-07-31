
#include <stdlib.h>
#include <stdio.h>		/* for fprintf(), stderr, BUFSIZ */
#include <errno.h>		/* declare errno */
#include <fcntl.h>		/* for flags for open() */
#include <string.h>		/* declare strerror() */
#include <unistd.h>		/* for ssize_t */
#include <sys/types.h>
#include <sys/stat.h>		/* for mode_t */

// ./get_args -i -lr 'hi there' -f fred.c

int main(int argc, char *argv[])
{
	int i;

	for (i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
			printf("option: %s\n", argv[i] + 1);
		else
			printf("argument %d: %s\n", i, argv[i]);
	}
		
	return 0;
}

