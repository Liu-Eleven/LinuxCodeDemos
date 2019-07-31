
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>		
#include <sys/types.h>
#include <sys/stat.h>		

#include<event.h>  
#include<event2/util.h>  

//
// gcc timer.c -levent -o timer
//


struct event ev;
struct timeval tv;


void time_cb(int fd, short event, void *argc)
{
	printf("timer wakeup\n");
	event_add(&ev, &tv); // reschedule timer
}


int main()
{
	struct event_base *base = event_init();
	tv.tv_sec = 3; // 10s period
	tv.tv_usec = 0;
	evtimer_set(&ev, time_cb, NULL);
	event_add(&ev, &tv);
	event_base_dispatch(base);
	
	return 0;
}


