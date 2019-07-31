#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

char *pathval = NULL;

int main(int argc, char **argv)
{
	int i;
	
	printf("\n-------------------------------------------------\n");
	
	// Look for PATH; if not present, supply a default value 
	if ((pathval = getenv("PATH")) == NULL){
		printf("env PATH=NULL\n");
	}else{
		printf("env PATH=%s\n", pathval);
	}
	
	printf("\n-------------------------------------------------\n");

	if (setenv("PATH", "/bin:/usr/bin:/usr/ucb", 1) != 0) {
		printf("set env failure.\n");

	}

	if ((pathval = getenv("PATH")) == NULL){
		printf("env PATH=NULL\n");
	}else{
		printf("env PATH=%s\n", pathval);
	}

	printf("\n-------------------------------------------------\n");
	
	putenv("USER=test");
	
	if ((pathval = getenv("USER")) == NULL){
		printf("env USER=NULL\n");
	}else{
		printf("env USER=%s\n", pathval);
	}

	setenv("USER", "abc", 1);
	printf("USER=%s\n", getenv("USER"));
	unsetenv("USER");
	printf("USER=%s\n", getenv("USER"));
	
	printf("\n-------------------------------------------------\n");


	if (environ != NULL)
	for (i = 0; environ[i] != NULL; i++)
		printf("%s\n", environ[i]);

	return 0;
}