#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>

// compile code
// gcc -D__USE_GNU uname.c -o test
// gcc -c -D_GNU_SOURCE -Wall -Wreturn-type uname.c
//

int main(int argc,char **argv) 
{
	int z;
	struct utsname u_name;

	z = uname(&u_name);

	if ( z == -1 ) {
		fprintf(stderr,"%s: uname(2)\n",
		strerror(errno));
		exit(1);
	}

	printf(" sysname[] = '%s';\n", u_name.sysname);
	printf(" nodename[] = '%s';\n", u_name.nodename);
	printf(" release[] = '%s';\n", u_name.release);
	printf(" version[] = '%s';\n", u_name.version);
	printf(" machine[] = '%s';\n", u_name.machine);
	
#if _UTSNAME_DOMAIN_LENGTH - 0
# ifdef __USE_GNU
	printf("domainname[] = '%s';\n", u_name.domainname);
# else
	printf("__domainame[] = '%s';\n", u_name.__domainname);
# endif
#endif	
	
	return 0;
}