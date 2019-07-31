#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "bmp.h"




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

	struct image *img;
	img = load_bmp("image.bmp");
	printf("\n\n\n");
	printf("width: %d  height: %d\n", img->width, img->height);
	printf("row_length: %d\n", img->row_length);
	printf("depth: %04d\n", img->bbp);
	printf("pixel_array_size: %d\n", img->pixel_array_size);

	int color = 0;
	int pos = 0;
	
	int offset_x, offset_y;
	offset_x = (480 - img->width) / 2;
	offset_y = (272 - img->height) / 2;
	printf("start_x:%d start_y:%d\n", offset_x, offset_y);
			
	for (y=0; y < img->height; y++) 
	{
		for (x=0; x < img->width; x++) 
		{
			pos = (x+ y * img->width) * img->bbp / 8 ;
			
			color = img->pixel_array[pos + 2] << 16 | 
				img->pixel_array[pos + 1] << 8 | 
				img->pixel_array[pos];
			fb_setpixel(480, 272, x, y, color);
		}
	}

	free_img(img->pixel_array, img);
	
	fb_deinit();

}

