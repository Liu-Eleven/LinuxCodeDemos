#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/fb.h>

static unsigned char*  npu8_fbmem;
static int             ns32_fb;
static unsigned int    fbw, fbh, fbd;
static unsigned int    nu32_screensize;

typedef struct
{
	int offset;
	int length;
} color_t;

typedef struct
{
	color_t red;
	color_t green;
	color_t blue;
} color_field_t;

color_field_t color_field;

static void* _fb_mmap(int fd, unsigned int screensize)
{
	caddr_t fbmem;

	if ((fbmem = mmap(0, screensize, PROT_READ | PROT_WRITE,
                      MAP_SHARED, fd, 0)) == MAP_FAILED)
	{
        perror(__func__);
        return (void *) (-1);
    }

    return fbmem;
}

static int _fb_munmap(void *start, size_t length)
{
    return (munmap(start, length));
}

static int _fb_stat(int fd, unsigned int *width, unsigned int *height, unsigned int *depth)
{
    //struct fb_fix_screeninfo fb_finfo;
    struct fb_var_screeninfo fb_vinfo;

    //if (ioctl(fd, FBIOGET_FSCREENINFO, &fb_finfo)) {
    //    perror(__func__);
    //    return -1;
    //}

    if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_vinfo)) {
        perror(__func__);
        return -1;
    }
	
	color_field.red.offset = fb_vinfo.red.offset;
	color_field.red.length = fb_vinfo.red.length;
	
	color_field.green.offset = fb_vinfo.green.offset;
	color_field.green.length = fb_vinfo.green.length;
	
	color_field.blue.offset = fb_vinfo.blue.offset;
	color_field.blue.length = fb_vinfo.blue.length;
	
	printf("red.offset %d  red.length %d | green.offset %d  green.length %d | blue.offset %d  blue.length %d \n", color_field.red.offset, color_field.red.length, color_field.green.offset, color_field.green.length, color_field.blue.offset, color_field.blue.length); 
     
	
#if 1	
    printf("xres %d\n", fb_vinfo.xres); 
    printf("yres %d\n", fb_vinfo.yres);          
    printf("xres_virturl %d\n",fb_vinfo.xres_virtual); 
    printf("yres_virtual %d\n",fb_vinfo.yres_virtual); 
    printf("xoffset %d\n",fb_vinfo.xoffset); //虚拟到可见的偏移        0
    printf("yoffset %d\n",fb_vinfo.yoffset);                         
    printf("bits_per_pixel %d\n",fb_vinfo.bits_per_pixel); //每像素位数 bpp  16
    printf("grayscale %d\n",fb_vinfo.grayscale);//非零时，指灰度
    
    printf("fb_bitfield red.offset %d\n",fb_vinfo.red.offset); 
    printf("fb_bitfield .length %d\n",fb_vinfo.red.length);        
    printf("fb_bitfield .msb_right %d\n",fb_vinfo.red.msb_right);   
    printf("fb_bitfield green.offset %d\n",fb_vinfo.green.offset);  
    printf("fb_bitfield .length %d\n",fb_vinfo.green.length);        
    printf("fb_bitfield .msb_right %d\n",fb_vinfo.green.msb_right);  
    printf("fb_bitfield blue.offset %d\n",fb_vinfo.blue.offset);
    printf("fb_bitfield .length %d\n",fb_vinfo.blue.length);
    printf("fb_bitfield .msb_right %d\n",fb_vinfo.blue.msb_right);
    printf("fb_bitfield transp.offset %d\n",fb_vinfo.transp.offset);
    printf("fb_bitfield .length %d\n",fb_vinfo.transp.length);
    printf("fb_bitfield .msb_right %d\n",fb_vinfo.transp.msb_right);

    printf("nonstd %d\n",fb_vinfo.nonstd); //!=0 非标准像素格式
    printf("activate %d\n",fb_vinfo.activate);
    printf("height %d\n",fb_vinfo.height); 
    printf("widht %d\n",fb_vinfo.width);             
    printf("accel_flags %d\n",fb_vinfo.accel_flags); //看 fb_info.flags

    //定时，除了 pixclock之外，其他的都以像素时钟为单位
    printf("pixclock %d\n",fb_vinfo.pixclock);//像素时钟，皮秒   80000
    printf("left_margin %d\n",fb_vinfo.left_margin);//行切换：从同步到绘图之间的延迟    28
    printf("right_margin %d\n",fb_vinfo.right_margin);//行切换：从绘图到同步之间的延迟   24
    printf("upper_margin %d\n",fb_vinfo.upper_margin);//帧切换：从同步到绘图之间的延迟   6
    printf("lower_margin %d\n",fb_vinfo.lower_margin);//帧切换：从绘图到同步之间的延迟    2

    printf("hsync_len %d\n",fb_vinfo.hsync_len); //hor 水平同步的长度         42
    printf("vsync_len %d\n",fb_vinfo.vsync_len); //vir 垂直同步的长度         12

    printf("sync %d\n",fb_vinfo.sync); //
    printf("vmode %d\n",fb_vinfo.vmode);
    printf("rotate %d\n", fb_vinfo.rotate);
#endif	

    *width  = fb_vinfo.xres;
    *height = fb_vinfo.yres;
    *depth  = fb_vinfo.bits_per_pixel;

    return 0;
}



int fb_init(void)
{ 
	ns32_fb = open("/dev/fb0", O_RDWR);
	if(ns32_fb<0)
	{
		printf("can not open\n");
		return -1;
	}

	if( _fb_stat(ns32_fb, &fbw, &fbh, &fbd) < 0 )
		return -1;

	printf("%d, %d, %d\n", fbw, fbh, fbd);
	nu32_screensize = fbw * fbh * fbd / 8;
	npu8_fbmem = _fb_mmap(ns32_fb, nu32_screensize);

	return 0;
}

void fb_deinit(void)
{ 
	close(ns32_fb);
	_fb_munmap(npu8_fbmem, nu32_screensize);
}


int rgb565_to_bgr565(int rgb565)
{
	int r,g,b;
	r = (rgb565 >> 11)  &0x1f;
	g = (rgb565 >> 6) & 0x3f;
	b = (rgb565 >> 0) & 0x1f;
	return r + (g<<6) + (b<<11);
}

int bgr565_to_rgb565(int brg565)
{
	int r,g,b;
	b = (brg565 >> 11)  &0x1f;
	g = (brg565 >> 6) & 0x3f;
	r = (brg565 >> 0) & 0x1f;
	return b + (g<<6) + (r<<11);
}

int fb_setpixel(int width, int height, int x, int y, unsigned int color24)
{
	int color,r,g,b;
	
	if ((x > width) || (y > height))
        return -1;

	r = (color24 >> (16 + (8 - color_field.red.length)))  & ((1 << color_field.red.length) - 1);
	g = (color24 >> (8 + (8 - color_field.green.length))) & ((1 << color_field.green.length) - 1);
	b = (color24 >> (0 + (8 - color_field.blue.length))) & ((1 << color_field.blue.length) - 1);
	color = (r << color_field.red.offset) + (g << color_field.green.offset ) + (b << color_field.blue.offset );
	
	if(fbd < 24)
	{
		unsigned short *dst = ((unsigned short *)(npu8_fbmem + (y * width + x) * fbd / 8));
		*dst = color;
	}
	else
	{
		unsigned long *dst = ((unsigned long *)(npu8_fbmem + (y * width + x) * fbd / 8));
		*dst = color;
	}
    
    return 0;
}

unsigned long fb_readpixel(int width, int height, int x, int y)
{
	unsigned long color = 0;
	
	if ((x > width) || (y > height))
		return -1;
	
	if(fbd < 24)
	{
		unsigned short *dst = ((unsigned short *)(npu8_fbmem + (y * width + x) * fbd / 8));
		color = *dst;
	}
	else
	{
		unsigned long *dst = ((unsigned long *)(npu8_fbmem + (y * width + x) * fbd / 8));
		color = *dst;
	}
	
	return color;
}

