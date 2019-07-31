#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>

char name[512] = {0};

int main(int argc,char **argv) 
{
	int z;
	struct utsname u_name;
	memset(name, 0, sizeof(name));
	gethostname(name, sizeof(name));

	printf(" hostname = '%s';\n", name);
	
	return 0;
}