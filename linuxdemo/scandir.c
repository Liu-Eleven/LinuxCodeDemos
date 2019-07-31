#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>	 /* for printf() etc. */
#include <errno.h>	 /* for errno */
#include <sys/types.h> /* for system types */
#include <dirent.h>	/* for directory functions */

void printdir(char *dir, int depth)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

	if ((dp = opendir(dir)) == NULL)
	{
		fprintf(stderr, "%s: cannot open for reading: %s\n", dir, strerror(errno));
		return;
	}
	chdir(dir);
	errno = 0;
	while ((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if(S_ISDIR(statbuf.st_mode))
		{
			if(strcmp(".", entry->d_name) == 0 || 
				strcmp("..", entry->d_name) == 0)
				continue;
			printf("%*s%s/\n", depth, "", entry->d_name);
			printdir(entry->d_name, depth+4);
		}
		else
			printf("%*s%s\n", depth, "", entry->d_name);
	}
	chdir("..");

	if (errno != 0)
	{
		fprintf(stderr, "%s: reading directory entries: %s\n", dir, strerror(errno));
		return;
	}

	if (closedir(dp) != 0)
	{
		fprintf(stderr, "%s: closedir: %s\n", dir, strerror(errno));
		return;
	}

}

int main(int argc, char **argv)
{
	int i;

	if (argc == 1)
		printdir(".", 0); /* default to current directory */
	else
		for (i = 1; i < argc; i++)
			printdir(argv[i], 0);

	return 0;
}
