
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>	
#include <sys/utsname.h>	
#include <sys/types.h>
#include <sys/stat.h>		


int main(int argc, char **argv)
{
	char computer[256];
	struct utsname uts;

	if(gethostname(computer, 255) != 0 || uname(&uts) < 0)
	{
		fprintf(stderr, "could not get host information\n");
		exit(1);
	}

	printf("computer host name is %s\n", computer);
	printf("system is %s on %s hardware\n", uts.sysname, uts.machine);
	printf("nodename is %s\n", uts.nodename);
	printf("version is %s, %s\n", uts.release, uts.version);

	return 0;
}


