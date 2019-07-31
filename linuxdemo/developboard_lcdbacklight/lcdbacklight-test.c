#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

#define LCD_BACKLIGHT_ON    0x1
#define LCD_BACKLIGHT_OFF    0x0

int main(void)
{
	int fd;
	char buf[8];
	
	fd = open("/dev/lcdbacklight", 0);
	
	if (fd < 0) 
	{
		perror("open device /dev/lcdbacklight");
		exit(1);
	}

	buf[0]=LCD_BACKLIGHT_ON;
	write(fd, buf, 1);
	
	sleep(3);
	
	buf[0]=LCD_BACKLIGHT_OFF;
	write(fd, buf, 1);
	
	close(fd);
	
	return 0;
}

