#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int main(int argc, char **argv)
{
	char systime[64] = {0};
	char sysdate[64] = {0};
	
	time_t ltime;
	struct tm* today;	//year = tm_year + 1900; month	= tm_mon + 1;
	char time_fmt[20];
	char date_fmt[20];
	
	if(argc != 3)
	{
		printf("Usage:\n");
		printf("%s <time_format: time_24/time_12> <date_format: date_ymd/date_mdy/date_dmy>\n", argv[0]);
		return 0;
	}
	
	time(&ltime);
	today = localtime(&ltime);
	
	memcpy(time_fmt, argv[1], sizeof(time_fmt));
	memcpy(date_fmt, argv[2], sizeof(date_fmt));
	
	strftime(systime, sizeof(systime), "%Y%m%d_%H%M%S", today);       //24小时制
	printf("DATE_TIME: %s\n", systime);
		
		
	//输出时间
	if(strcmp(time_fmt, "time_24") == 0)
	{
		strftime(systime, sizeof(systime), " %H:%M:%S", today);       //24小时制
		printf("TIME: %s\n", systime);
	}
	else if(strcmp(time_fmt, "time_12") == 0)
	{
		strftime(systime, sizeof(systime), "%I:%M:%S %p", today);    //12小时制
		printf("TIME: %s\n", systime);
	}
	else
	{
		printf("Unknown time_fmt!\n");
	}
	
	//输出日期
	if(strcmp(date_fmt, "date_ymd") == 0)
	{
		strftime(sysdate, sizeof(sysdate), "%Y/%m/%d", today);    //年-月-日
		printf("DATE: %s\n", sysdate);
	}
	else if(strcmp(date_fmt, "date_mdy") == 0)
	{
		strftime(sysdate, sizeof(sysdate), "%m/%d/%Y", today);    //月-日-年
		printf("DATE: %s\n", sysdate);
	}
	else if(strcmp(date_fmt, "date_dmy") == 0)
	{
		strftime(sysdate, sizeof(sysdate), "%d/%m/%Y", today);    //日-月-年
		printf("DATE: %s\n", sysdate);
	}
	else
	{
		printf("Unknown date_fmt!\n");
	}
 
	return 0;
}