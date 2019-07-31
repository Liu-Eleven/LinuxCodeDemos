#include <sys/types.h>

int            fb_init(void);
void           fb_deinit(void);
int            fb_setpixel(int width, int height, int x, int y, unsigned int color);
unsigned long fb_readpixel(int width, int height, int x, int y);
