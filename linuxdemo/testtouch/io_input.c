
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <linux/input.h>

#include "io_input.h"

#ifndef EV_SYN /* 2.4 kernel headers */
# define EV_SYN 0x00
#endif

static int ts_fd = -1;
int current_x = -1;
int current_y = -1;
unsigned int current_p = 0;


///////////////////////////////////////////////////////

int ts_open(const char *name, int nonblock)
{
	int flags = O_RDONLY;

	if (nonblock)
		flags |= O_NONBLOCK;

	ts_fd = open(name, flags);
	if (ts_fd == -1)
	{
		printf("open touch screen driver fail!\n");
	}

	return ts_fd;
}


int ts_close(void)
{
	int ret = -1;
	if(ts_fd != -1)
		ret = close(ts_fd);
	return ret;
}


int ts_read(Tts_sample *samp, int nr)
{
	struct input_event ev;
	int ret = nr;
	int total = 0;

	if(ts_fd == -1)
	{
		printf("ts_read()-->ts_fd=%d!\n", ts_fd);
		return -1;
	}

#ifdef EV_SYN
	while (total < nr) 
	{
		ret = read(ts_fd, &ev, sizeof(struct input_event));
		if (ret < sizeof(struct input_event)) 
		{
			total = -1;
			break;
		}

		switch (ev.type) 
		{
		case EV_SYN:
			/* Fill out a new complete event */
			samp->x = current_x;
			samp->y = current_y;
			samp->pressure = current_p;
			samp->tv = ev.time;

			//printf("RAW----> %d %d %d %d.%d\n",
			//	samp->x, samp->y, samp->pressure, samp->tv.tv_sec, samp->tv.tv_usec);

			samp++;
			total++;
			break;
		case EV_ABS:
			switch (ev.code) 
			{
			case ABS_X:
				current_x = ev.value;
				break;
			case ABS_Y:
				current_y = ev.value;
				break;
			case ABS_PRESSURE:
				current_p = ev.value;
				break;
			}
			break;
		}
	}
	ret = total;
#else   
	static int curr_x = 0, curr_y = 0, curr_p = 0;
	static int got_curr_x = 0, got_curr_y = 0;
	int got_curr_p = 0;
	int next_x, next_y;
	int got_next_x = 0, got_next_y = 0;
	int got_tstamp = 0;

	while (total < nr) 
	{
		ret = read(ts_fd, &ev, sizeof(struct input_event));
		if (ret < sizeof(struct input_event)) break;

		/*
		 * We must filter events here.  We need to look for
		 * a set of input events that will correspond to a
		 * complete ts event.  Also need to be aware that
		 * repeated input events are filtered out by the kernel.
		 * 
		 * We assume the normal sequence is: 
		 * ABS_X -> ABS_Y -> ABS_PRESSURE
		 * If that sequence goes backward then we got a different
		 * ts event.  If some are missing then they didn't change.
		 */
		if (ev.type == EV_ABS) 
		switch (ev.code) 
		{
		case ABS_X:
			if (!got_curr_x && !got_curr_y) 
			{
				got_curr_x = 1;
				curr_x = ev.value;
			} 
			else 
			{
				got_next_x = 1;
				next_x = ev.value;
			}
			break;
		case ABS_Y:
			if (!got_curr_y) 
			{
				got_curr_y = 1;
				curr_y = ev.value;
			} 
			else 
			{
				got_next_y = 1;
				next_y = ev.value;
			}
			break;
		case ABS_PRESSURE:
			got_curr_p = 1;
			curr_p = ev.value;
			break;
		}

		/* go back if we just got irrelevant events so far */
		if (!got_curr_x && !got_curr_y && !got_curr_p) 
			continue;

		/* time stamp with the first valid event only */
		if (!got_tstamp) 
		{
			got_tstamp = 1;
			samp->tv = ev.time;
		}

		if ( (!got_curr_x || !got_curr_y) && !got_curr_p &&
		     !got_next_x && !got_next_y ) 
		{
			/*
			 * The current event is not complete yet.
			 * Give the kernel a chance to feed us more.
			 */
			struct timeval tv = {0, 0};
			fd_set fdset;
			FD_ZERO(&fdset);
			FD_SET(ts_fd, &fdset);
			ret = select(ts_fd+1, &fdset, NULL, NULL, &tv);
		       	if (ret == 1) continue;
			if (ret == -1) break;
		}

		/* We consider having a complete ts event */
		samp->x = curr_x;
		samp->y = curr_y;
		samp->pressure = curr_p;

        //printf("RAW--x--> %d %d %d\n",samp->x,samp->y,samp->pressure);

		samp++;
		total++;
        
		/* get ready for next event */
		if (got_next_x) 
			curr_x = next_x; 
		else 
			got_curr_x = 0;
		
		if (got_next_y) 
			curr_y = next_y; 
		else 
			got_curr_y = 0;
		
		got_next_x = got_next_y = got_tstamp = 0;
	}

	if (ret) 
		ret = -1;
	
	if (total) 
		ret = total;
#endif
	return ret;
}




