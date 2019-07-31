#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char **argv)
{
	union{
		short s;
		char c[sizeof(short)];
	}un;
	
	un.s = 0x0102;

	if (sizeof(short) == 2){
		if (un.c[0] == 0x01 && un.c[1] == 0x02){
			printf("big-endian\n");
		}else if (un.c[0] == 0x02 && un.c[1] == 0x01){
			printf("little-endian\n");
		}else{
			printf("unknown\n");
		}
	}else{
		printf("sizeof(short)=%d\n", sizeof(short));
	}
	
	exit(0);

	return 0;
}