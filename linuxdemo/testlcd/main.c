#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>


void drawhline  (int x0, int y,  int x1, int color) 
{
	for (; x0 <= x1; x0++) 
	{
		fb_setpixel(480, 272, x0, y, color);
    }
}


void draw_vline  (int x, int y0,  int y1, int color) 
{
	for (; y0 <= y1; y0++) 
	{
		fb_setpixel(480, 272, x, y0, color);
    }
}


void fill_rect(int x0, int y0, int x1, int y1, int color) 
{
	int x, y;
	
	for (y=y0; y < y1; y++) 
	{
		for (x=x0; x < x1; x++) 
		{
			fb_setpixel(480, 272, x, y, color);
		}
	}
}

int main()
{
	int i,x,y;

	fb_init();
	
	fill_rect(0, 0, 480, 272 , 0x0);
	
	while(1)
	{
#if 1	
		fill_rect(0, 0, 160, 272, 0xff0000);
		fill_rect(160, 0, 320, 272, 0xff00);
		fill_rect(320, 0, 480, 272, 0xff);
#else
		fill_rect(0, 0, 480, 272, 0xff0000);//fill_rect(0, 0, 480, 272, 0xff);
#endif		
	}

	fb_deinit();

}

