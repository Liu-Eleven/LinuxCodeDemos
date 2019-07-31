#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/fb.h>

#include "font.h"
#include "fbutils.h"

union multiptr {
	unsigned char *p8;
	unsigned short *p16;
	unsigned long *p32;
};

static int fb_fd=0;

static struct fb_fix_screeninfo fix;
static struct fb_var_screeninfo var;

static unsigned char *line_addr;

static int bytes_per_pixel;
__u32 xres, yres;


static unsigned colormap [256];

int open_framebuffer(void)
{
	fb_fd = open("/dev/fb0", O_RDWR);

	if (fb_fd == -1) {
		perror("open fbdevice");
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix) < 0) {
		perror("ioctl FBIOGET_FSCREENINFO");
		close(fb_fd);
		return -1;
	}

	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &var) < 0) {
		perror("ioctl FBIOGET_VSCREENINFO");
		close(fb_fd);
		return -1;
	}
	xres = var.xres;
	yres = var.yres;
	bytes_per_pixel = (var.bits_per_pixel + 7) / 8;


	line_addr = mmap(NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fb_fd, 0);
	if (line_addr == (unsigned char *)-1) {
		perror("mmap framebuffer");
		close(fb_fd);
		return -1;
	}

	memset(line_addr,0,fix.smem_len);

	return 0;
}

void close_framebuffer(void)
{
	munmap(line_addr, fix.smem_len);
	close(fb_fd);

}

void put_cross(int x, int y, unsigned colidx)
{
	line (x - 10, y, x - 2, y, colidx);
	line (x + 2, y, x + 10, y, colidx);
	line (x, y - 10, x, y - 2, colidx);
	line (x, y + 2, x, y + 10, colidx);

#if 1
	line (x - 6, y - 9, x - 9, y - 9, colidx + 1);
	line (x - 9, y - 8, x - 9, y - 6, colidx + 1);
	line (x - 9, y + 6, x - 9, y + 9, colidx + 1);
	line (x - 8, y + 9, x - 6, y + 9, colidx + 1);
	line (x + 6, y + 9, x + 9, y + 9, colidx + 1);
	line (x + 9, y + 8, x + 9, y + 6, colidx + 1);
	line (x + 9, y - 6, x + 9, y - 9, colidx + 1);
	line (x + 8, y - 9, x + 6, y - 9, colidx + 1);
#else
	line (x - 7, y - 7, x - 4, y - 4, colidx + 1);
	line (x - 7, y + 7, x - 4, y + 4, colidx + 1);
	line (x + 4, y - 4, x + 7, y - 7, colidx + 1);
	line (x + 4, y + 4, x + 7, y + 7, colidx + 1);
#endif
}


void put_char(int x, int y, int c, int colidx)
{
	int i,j,bits;

	for (i = 0; i < font_vga_8x8.height; i++) {
		bits = font_vga_8x8.data [font_vga_8x8.height * c + i];
		for (j = 0; j < font_vga_8x8.width; j++, bits <<= 1)
			if (bits & 0x80)
				pixel (x + j, y + i, colidx);
	}
}

void put_string(int x, int y, char *s, unsigned colidx)
{
	int i;
	for (i = 0; *s; i++, x += font_vga_8x8.width, s++)
		put_char (x, y, *s, colidx);
}

void put_string_center(int x, int y, char *s, unsigned colidx)
{
	size_t sl = strlen (s);
        put_string (x - (sl / 2) * font_vga_8x8.width,
                    y - font_vga_8x8.height / 2, s, colidx);
}

void setcolor(unsigned colidx, unsigned value)
{
	unsigned res;
	unsigned short red, green, blue;
	struct fb_cmap cmap;

	switch (bytes_per_pixel) {
	default:
	case 1:
		res = colidx;
		red = (value >> 8) & 0xff00;
		green = value & 0xff00;
		blue = (value << 8) & 0xff00;
		cmap.start = colidx;
		cmap.len = 1;
		cmap.red = &red;
		cmap.green = &green;
		cmap.blue = &blue;
		cmap.transp = NULL;

        	if (ioctl (fb_fd, FBIOPUTCMAP, &cmap) < 0)
        	        perror("ioctl FBIOPUTCMAP");
		break;
	case 2:
	case 4:
		red = (value >> 16) & 0xff;
		green = (value >> 8) & 0xff;
		blue = value & 0xff;
		res = ((red >> (8 - var.red.length)) << var.red.offset) |
                      ((green >> (8 - var.green.length)) << var.green.offset) |
                      ((blue >> (8 - var.blue.length)) << var.blue.offset);
	}
        colormap [colidx] = res;
}

static inline void __setpixel (union multiptr loc, unsigned xormode, unsigned color)
{
	switch(bytes_per_pixel) {
	case 1:
	default:
		if (xormode)
			*loc.p8 ^= color;
		else
			*loc.p8 = color;
		break;
	case 2:
		if (xormode)
			*loc.p16 ^= color;
		else
			*loc.p16 = color;
		break;
	case 4:
		if (xormode)
			*loc.p32 ^= color;
		else
			*loc.p32 = color;
		break;
	}
}

void pixel (int x, int y, unsigned colidx)
{
	unsigned xormode;
	union multiptr loc;

	if ((x < 0) || ((__u32)x >= var.xres_virtual) ||
	    (y < 0) || ((__u32)y >= var.yres_virtual))
		return;

	xormode = colidx & XORMODE;
	colidx &= ~XORMODE;

	loc.p8 = line_addr + y*fix.line_length + x * bytes_per_pixel;
	__setpixel (loc, xormode, colormap [colidx]);
}

void line (int x1, int y1, int x2, int y2, unsigned colidx)
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
			pixel (x1 >> 16, y1, colidx);
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
			pixel (x1, y1 >> 16, colidx);
			y1 += dy;
			x1++;
		}
	}
}

void rect (int x1, int y1, int x2, int y2, unsigned colidx)
{
	line (x1, y1, x2, y1, colidx);
	line (x2, y1, x2, y2, colidx);
	line (x2, y2, x1, y2, colidx);
	line (x1, y2, x1, y1, colidx);
}

void fillrect (int x1, int y1, int x2, int y2, unsigned colidx)
{
	int tmp;
	unsigned xormode;
	union multiptr loc;

	/* Clipping and sanity checking */
	if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
	if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }
	if (x1 < 0) x1 = 0; if ((__u32)x1 >= xres) x1 = xres - 1;
	if (x2 < 0) x2 = 0; if ((__u32)x2 >= xres) x2 = xres - 1;
	if (y1 < 0) y1 = 0; if ((__u32)y1 >= yres) y1 = yres - 1;
	if (y2 < 0) y2 = 0; if ((__u32)y2 >= yres) y2 = yres - 1;

	if ((x1 > x2) || (y1 > y2))
		return;

	xormode = colidx & XORMODE;
	colidx &= ~XORMODE;

	colidx = colormap [colidx];

	for (; y1 <= y2; y1++) {
		loc.p8 = line_addr +y1*fix.line_length + x1 * bytes_per_pixel;
		for (tmp = x1; tmp <= x2; tmp++) {
			__setpixel (loc, xormode, colidx);
			loc.p8 += bytes_per_pixel;
		}
	}
}
