
#include <stdio.h>

int main(void)
{
	union i_f {
		int i;
		float f;
	} u;

	u.f = 12.34;
	printf("%f also looks like %#x\n", u.f, u.i);

	union endian{
		unsigned long ivalue;
		unsigned char byte[3];
	}endian_u;

	endian_u.ivalue = 0x04030201;
	printf("[0]=%02x [1]=%02x [2]=%02x [3]=%02x\n", endian_u.byte[0], endian_u.byte[1], endian_u.byte[2], endian_u.byte[3]);
	if(endian_u.byte[0] == 0x01)
		printf("little endian\n");
	else
		printf("big endian\n");

	return 0;
}
