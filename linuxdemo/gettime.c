
#include <stdlib.h>
#include <stdio.h>		
#include <errno.h>		
#include <fcntl.h>		
#include <string.h>		
#include <unistd.h>		
#include <sys/types.h>
#include <sys/stat.h>	
#include <time.h>	

#define BST (+1)
#define CCT (+8)

#if 0
printf("London：%2d:%02d\n", (info->tm_hour+BST)%24, info->tm_min);
printf("china：%2d:%02d\n", (info->tm_hour+CCT)%24, info->tm_min);
#endif

int main(int argc, char *argv[])
{
	int i;
	time_t the_time;
#if 0
struct tm {
   int tm_sec;         /* 秒，范围从 0 到 59                */
   int tm_min;         /* 分，范围从 0 到 59                */
   int tm_hour;        /* 小时，范围从 0 到 23                */
   int tm_mday;        /* 一月中的第几天，范围从 1 到 31                    */
   int tm_mon;         /* 月份，范围从 0 到 11                */
   int tm_year;        /* 自 1900 起的年数                */
   int tm_wday;        /* 一周中的第几天，范围从 0 到 6                */
   int tm_yday;        /* 一年中的第几天，范围从 0 到 365                    */
   int tm_isdst;       /* 夏令时                        */    
};
#endif
	struct tm *tm_ptr;

	for(i=1; i<10; i++){
		time(&the_time);
		tm_ptr = gmtime(&the_time);

		printf("raw time is %ld ", the_time);
		printf("date: %04d/%02d/%02d ", tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday);
		printf("time: %02d:%02d:%02d\n", (tm_ptr->tm_hour+CCT)%24, tm_ptr->tm_min, tm_ptr->tm_sec);
		sleep(1);
	}

	exit(0);
	
	return 0;
}

