#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#include "io_input.h"

static unsigned int pressure = 0;

void test(void)
{
 	static int x, y;		
	Tts_sample samp;
	int ret = -1;

	ret = ts_read(&samp, 1);

	if (ret < 0)
	{
		x = y = -1;
	}

	if (ret != 1)
	{
		x = y = -1;
	}
	else
	{
		if(samp.pressure == 1) // down state
		{
			x = samp.x;
			y = samp.y;
		}
		else // up state
		{
			x = -1;
			y = -1;
		}

		if(pressure != samp.pressure)
		{
			pressure = samp.pressure;
			printf("%6d %6d %6d\n\n", x, y, samp.pressure);
		}
	}
}

int main()
{
	int fd;
	
	if( (fd = ts_open("/dev/event0", 1)) == -1)
	{
    	fd = ts_open("/dev/input/event0", 1);
	}

	if (fd == -1)
	{
		printf("open touch screen driver fail!\n");
	}
	
	while(1)
	{
		test();
		sleep(1);
	}

	ts_close();

}

