
#include <stdio.h>
#include <limits.h>


int main (void)
{
	FILE *in, *out;
	struct pirate
	{
		char name[100]; /* real name */
		unsigned long booty; /* in pounds sterling */
		unsigned int beard_len; /* in inches */
	} p, blackbeard = { "Edward Teach", 950, 48 };
	
	out = fopen("data", "w");
	if (!out)
	{
		perror("fopen");
		return 1;
	}

	if(!fwrite(&blackbeard, sizeof(struct pirate), 1, out))
	{
		perror("fwrite");
		return 1;
	}

	if (fputc('[', out) == EOF)
	{
		perror("fputc");
		return 1;
	}
	
	if (fputs("The ship is made of wood.\n", out) == EOF)
	{
		perror("fputs");
		return 1;
	}
	
	if (fputc(']', out) == EOF)
	{
		perror("fputc");
		return 1;
	}
	
	if(fclose(out))
	{
		perror("fclose");
		return 1;
	}
	
	in = fopen("data", "r");
	if (!in)
	{
		perror("fopen");
		return 1;
	}
	if (!fread(&p, sizeof(struct pirate), 1, in))
	{
		perror("fread");
		return 1;
	}
	printf("name=\"%s\" booty=%lu beard_len=%u\n", p.name, p.booty, p.beard_len);
	
	int c;
	if ((c = fgetc(in)) == EOF)
	{
		perror("fgetc");
		return 1;
	}
	printf("%c\n", (char)c);
	
	char buf[LINE_MAX];
	if (!fgets(buf, LINE_MAX, in))
	{
		perror("fgets");
		return 1;
	}
	printf("%s\n", buf);
	
	if ((c = fgetc(in)) == EOF)
	{
		perror("fgetc");
		return 1;
	}
	printf("%c\n", (char)c);

	if (fclose(in))
	{
		perror("fclose");
		return 1;
	}
	
	return 0;
}	


