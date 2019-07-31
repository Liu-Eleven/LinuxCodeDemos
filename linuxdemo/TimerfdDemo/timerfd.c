
#if 0

#include <sys/timerfd.h>
/*
 * 功能 ： 创建定时器
 * 返回值：成功返回定时器文件描述符，失败返回-1
 * 参数：  clockid可以是CLOCK_REALTIME（实时时钟）或者CLOCK_MONOTONIC（递增时钟），实时时钟可以被系统时间改变，后者不会。
 * 				 如果这里使用实时时钟，当手动更改系统时间定时器也会受影响，而递增时钟则只受设置的时间值影响。
 * flags : 可选项包括TFD_NONBLOCK（非阻塞）和TFD_CLOEXEC，阻塞指的是当定时器未超时的时候，如果调用read(timerfd)会阻塞直
 *  				到定时器超时，如果设置TFD_NONLOCK，则会直接返回并返回-1. 这与套接字描述符类似。
 */
int timerfd_create(int clockid, int flags); 


/*
 * 功能 定时器启动和关闭
 * 返回值 ： 成功返回0，失败返回-1，并存储错误码到errno
 * 参数： fd: 定时器描述符
 *	 flags: 0 或者TFD_TIMER_ABSTIME，0代表相对时间，即相对于当前时间多少，后者是绝对时间。
 * 	 new_value: 当new_value.it_value非0时，用于设置定时器第一次超时时间,为0代表停止定时器
		    new_value.it_interval:表示第一次超时后下一次超时的时间，为0代表定时器只超时一次
 *	 old_value: 如果不为NULL，则用来存储当前时间。
 */
int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);  


/*
 * 功能: 和普通描述符一样，用完后使用close释放
 * 参数：timerfd为timerfd_create()创建的定时器描述符
 */
close(timerfd);

//函数参数中数据结构如下： 
struct timespec 
{ 
    time_t tv_sec; /* Seconds */ 
    long tv_nsec; /* Nanoseconds */ 
}; 
struct itimerspec 
{ 
    struct timespec it_interval; /* Interval for periodic timer */ 
    struct timespec it_value; /* Initial expiration */ 
};


#endif

//
// 编译运行：编译时要加rt库(g++ -lrt timerfd.cc -o timerfd)
//

 
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include<errno.h>
#include <time.h>
 
#define TIME_MAX 2 
#define log(fmt, arg...) printf(""fmt, ##arg)
 
void main() 
{
    int tfd;    //定时器描述符
    int efd;    //epoll描述符
    int fds, ret;
    uint64_t value;
    struct epoll_event ev, *evptr;
    struct itimerspec time_intv; //用来存储时间
 
    tfd = timerfd_create(CLOCK_MONOTONIC, 0);   //创建定时器
    if(tfd == -1) {
        log("create timer fd fail \r\n");
        return ;
    }
 
    time_intv.it_value.tv_sec = TIME_MAX; //设定2s超时
    time_intv.it_value.tv_nsec = 0;
    time_intv.it_interval.tv_sec = time_intv.it_value.tv_sec;   //每隔2s超时
    time_intv.it_interval.tv_nsec = time_intv.it_value.tv_nsec;
 
    log("timer start ...\n");
    timerfd_settime(tfd, 0, &time_intv, NULL);  //启动定时器
    
    efd = epoll_create1(0); //创建epoll实例
    if (efd == -1) {
        log("create epoll fail \r\n");
        close(tfd);
        return ;
    }
    
    evptr = (struct epoll_event *)calloc(1, sizeof(struct epoll_event));
    if (evptr == NULL) {
        log("epoll event calloc fail \r\n");
        close(tfd);
        close(efd);
        return ;
    }
 
    ev.data.fd = tfd; 
    ev.events = EPOLLIN;    //监听定时器读事件，当定时器超时时，定时器描述符可读。
    epoll_ctl(efd, EPOLL_CTL_ADD, tfd, &ev); //添加到epoll监听队列中
 
    while(1) {
        fds = epoll_wait(efd, evptr, 1, -1);    //阻塞监听，直到有事件发生
        if(evptr[0].events & EPOLLIN){   
                ret = read(evptr->data.fd, &value, sizeof(uint64_t));
                if (ret == -1) 
                    log("read return -1, errno :%d \r\n", errno);
                else
                    log("*** timer up  *** \n");               
       }            
    }
 
    return ;
}


