
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>	
#include <sys/utsname.h>	
#include <sys/types.h>
#include <sys/stat.h>		

char *menu[] = {
	"a - add new record",
	"d - delete record",
	"q - quit",
	NULL,
};

int getchoice(char *greet, char *choices[])
{
	int chosen = 0;
	int selected = 0;
	char **option;

	do{
		printf("Choice: %s\n", greet);
		option = choices;
		while(*option){
			printf("%s\n", *option);
			option++;
		}
		
		do{
			selected = getchar();
		}while(selected == 0x0D || selected == 0x0A);
		
		option = choices;
		while(*option){
			if(selected == *option[0]){
				chosen = 1;
				break;
			}
			option++;
		}
		if(!chosen){
			printf("incorrect choice: %d, select again\n", selected);
		}
	}while(!chosen);
	return selected;
}

int main(int argc, char **argv)
{
	int choice = 0;

	do{
		choice = getchoice("please select an action", menu);
		printf("you have chosen: %c\n", choice);
	}while(choice != 'q');

	exit(0);

	return 0;
}


