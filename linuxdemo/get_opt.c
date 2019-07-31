
#include <stdlib.h>
#include <stdio.h>		/* for fprintf(), stderr, BUFSIZ */
#include <errno.h>		/* declare errno */
#include <fcntl.h>		/* for flags for open() */
#include <string.h>		/* declare strerror() */
#include <unistd.h>		/* for ssize_t */
#include <sys/types.h>
#include <sys/stat.h>		/* for mode_t */

// ./get_args -i -lr 'hi there' -f fred.c -q

int main(int argc, char *argv[])
{
	int opt;

	while((opt = getopt(argc, argv, ":if:lr")) != -1)
	{
		switch(opt)
		{
			case 'i':
			case 'l':
			case 'r':
				printf("option:%c\n", opt);
				break;
			case 'f':
				printf("filename:%s\n", optarg);
				break;
			case ':':
				printf("option needs a value\n");
				break;
			case '?':
				printf("unknown option:%c\n", opt);
				break;
		}

	}
	for ( ; optind < argc; optind++)
	{
		printf("argument: %s\n", argv[optind]);
	}
		
	return 0;
}

