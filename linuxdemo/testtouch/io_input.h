
#ifndef __IO_INPUT__
#define __IO_INPUT__

#include <sys/time.h>

typedef struct tag_ts_sample 
{
	int		x;
	int		y;
	unsigned int	pressure;
	struct timeval	tv;
}Tts_sample;

int ts_open(const char *name, int nonblock);
int ts_close();
int ts_read(Tts_sample *samp, int nr);

#endif


