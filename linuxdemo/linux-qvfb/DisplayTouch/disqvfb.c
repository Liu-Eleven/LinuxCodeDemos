#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "disqvfb.h"
#include "font.h"

static QVFbHeader *hdr;

void pixel(int x,int y,int color)
{
  int *buffer;
  char *data = (char *)hdr + hdr->dataoffset;
  buffer=(int *)(data+hdr->linestep*y+x*hdr->depth/8);
  *buffer=color;
}


void qvfb_update(int left,int top,int right,int bottom)
{
    hdr->update.left=left;
    hdr->update.top=top;
    hdr->update.right=right;
    hdr->update.bottom=bottom;
    
    hdr->dirty=1;
}

int dis_qvfb_init(void)
{
    key_t key;
    int shmid;

    key = ftok ("/tmp/.qtvfb_mouse-0", 'b');

    shmid = shmget (key, 0, 0);
    if (shmid != -1)
        hdr= (QVFbHeader*)shmat (shmid, 0, 0);

    if ((int)hdr == -1 || hdr== NULL)
    {
        return -1;
    }


    printf("width     :%d\n",hdr->width);
    printf("height    :%d\n",hdr->height);
    printf("depth     :%d\n",hdr->depth);
    printf("dataoffset:%d\n",hdr->dataoffset);

    return 0;
}

void dis_qvfb_exit(void)
{
    shmdt (hdr);
}

void qvfb_clear(int color)
{
  int i,j;
  
  for(i=0;i<hdr->width;i++)
    for(j=0;j<hdr->height;j++)
      pixel(i,j,color);

  qvfb_update(0,0,hdr->width,hdr->height);

}

void put_cross(int x, int y, int color)
{
	line (x - 10, y, x - 2, y, color);
	line (x + 2, y, x + 10, y, color);
	line (x, y - 10, x, y - 2, color);
	line (x, y + 2, x, y + 10, color);

	line (x - 6, y - 9, x - 9, y - 9, color + 100);
	line (x - 9, y - 8, x - 9, y - 6, color + 100);
	line (x - 9, y + 6, x - 9, y + 9, color + 100);
	line (x - 8, y + 9, x - 6, y + 9, color + 100);
	line (x + 6, y + 9, x + 9, y + 9, color + 100);
	line (x + 9, y + 8, x + 9, y + 6, color + 100);
	line (x + 9, y - 6, x + 9, y - 9, color + 100);
	line (x + 8, y - 9, x + 6, y - 9, color + 100);
}


void put_char(int x, int y, int c, int color)
{
	int i,j,bits;

	for (i = 0; i < font_vga_8x8.height; i++) {
		bits = font_vga_8x8.data [font_vga_8x8.height * c + i];
		for (j = 0; j < font_vga_8x8.width; j++, bits <<= 1)
			if (bits & 0x80)
				pixel (x + j, y + i, color);
	}
}

void put_string(int x, int y, char *s, int color)
{
	int i;
	for (i = 0; *s; i++, x += font_vga_8x8.width, s++)
		put_char (x, y, *s, color);
}

void put_string_center(int x, int y, char *s, int color)
{
	size_t sl = strlen (s);
        put_string (x - (sl / 2) * font_vga_8x8.width,
                    y - font_vga_8x8.height / 2, s, color);
}

void line (int x1, int y1, int x2, int y2, int color)
{
	int tmp;
	int dx = x2 - x1;
	int dy = y2 - y1;

	if (abs (dx) < abs (dy)) {
		if (y1 > y2) {
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		x1 <<= 16;
		/* dy is apriori >0 */
		dx = (dx << 16) / dy;
		while (y1 <= y2) {
			pixel (x1 >> 16, y1, color);
			x1 += dx;
			y1++;
		}
	} else {
		if (x1 > x2) {
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		y1 <<= 16;
		dy = dx ? (dy << 16) / dx : 0;
		while (x1 <= x2) {
			pixel (x1, y1 >> 16, color);
			y1 += dy;
			x1++;
		}
	}
}

void rect (int x1, int y1, int x2, int y2, int color)
{
	line (x1, y1, x2, y1, color);
	line (x2, y1, x2, y2, color);
	line (x2, y2, x1, y2, color);
	line (x1, y2, x1, y1, color);
}

void fillrect (int x1, int y1, int x2, int y2, int color)
{
  int x,y;

  if ((x1 > x2) || (y1 > y2))
      return;

  for (y=y1; y <y2; y++)
     for(x = x1; x <x2; x++)
       pixel (x,y, color);
}

