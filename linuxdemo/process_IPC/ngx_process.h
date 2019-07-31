
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */

#ifndef _NGX_PROCESS_H_INCLUDED_
#define _NGX_PROCESS_H_INCLUDED_

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include<fcntl.h> 
#include <string.h>

#include <sys/wait.h>
#include <signal.h>
#include <sys/ioctl.h>

#include <time.h>  
#include <sys/time.h>  
#include <errno.h>
//#include <ngx_setaffinity.h>
//#include <ngx_setproctitle.h>

#define NGX_INT32_LEN   (sizeof("-2147483648") - 1)
#define NGX_INT64_LEN   (sizeof("-9223372036854775808") - 1)


#define PID_FILE  "./pid.txt"

typedef pid_t       ngx_pid_t;
typedef intptr_t        ngx_int_t;
typedef uintptr_t       ngx_uint_t;

#define  NGX_OK          0
#define  NGX_ERROR      -1
#define  NGX_AGAIN      -2
#define  NGX_BUSY       -3
#define  NGX_DONE       -4
#define  NGX_DECLINED   -5
#define  NGX_ABORT      -6

#define NGX_LINEFEED             "\x0a"

//父子进程管道控制
#define NGX_CMD_OPEN_CHANNEL   1
#define NGX_CMD_CLOSE_CHANNEL  2
#define NGX_CMD_QUIT           3
#define NGX_CMD_TERMINATE      4
#define NGX_CMD_REOPEN         5


#define NGX_MAX_PROCESSES         1024

#define NGX_PROCESS_NORESPAWN     -1  //子进程退出时,父进程不会再次重启
#define NGX_PROCESS_JUST_SPAWN    -2  //
#define NGX_PROCESS_RESPAWN       -3  //子进程异常退出时,父进程需要重启
#define NGX_PROCESS_JUST_RESPAWN  -4  //
#define NGX_PROCESS_DETACHED      -5  //热切换

#define NGX_PROCESS_SINGLE     0
#define NGX_PROCESS_MASTER     1
#define NGX_PROCESS_SIGNALLER  2
#define NGX_PROCESS_WORKER     3

#define NGX_INVALID_PID  -1


typedef void (*ngx_spawn_proc_pt) ( void *data);

typedef struct {
    pid_t               pid;//进程ID
    int                 status;//由waitpid 系统调用获取取得进程状态
    int                 channel[2];//由socketpair产生的用于父子进程间互相通信的socket 句柄，

    ngx_spawn_proc_pt       proc;
    void               *data;
    char               *name;

    unsigned            respawn:1;   //为1表示在重新生成子进程
    unsigned            just_spawn:1;//为1表示正在生成子进程
    unsigned            detached:1;  //为1时表示在进行父、子进程分离
    unsigned            exiting:1;   //为1时表示进程正在退出
    unsigned            exited:1;    //为1时表示进程已经退出
} ngx_process_t;

typedef struct {
    char         *path;
    char         *name;
    char *const  *argv;
    char *const  *envp;
} ngx_exec_ctx_t;



pid_t spawn_process(ngx_spawn_proc_pt proc, void *data, char *name, int respawn);
void ngx_master_process_cycle();

void  ngx_signal_worker_processes(int signo);
ngx_int_t ngx_os_signal_process(char *name, ngx_pid_t pid);

ngx_pid_t ngx_execute(ngx_exec_ctx_t *ctx);
ngx_int_t ngx_init_signals();


/*
#if (NGX_HAVE_SCHED_YIELD)
#define ngx_sched_yield()  sched_yield()
#else
#define ngx_sched_yield()  usleep(1)
#endif
*/

extern int            ngx_argc;
extern char         **ngx_argv;
extern char         **ngx_os_argv;


extern int            ngx_channel;
extern ngx_int_t            ngx_process_slot;
extern ngx_int_t            ngx_last_process;
extern ngx_process_t  ngx_processes[NGX_MAX_PROCESSES];


extern ngx_uint_t      ngx_process;
extern ngx_uint_t      ngx_worker;
extern ngx_pid_t       ngx_pid;
extern ngx_pid_t       ngx_new_binary;
extern ngx_uint_t      ngx_inherited;
extern ngx_uint_t      ngx_daemonized;
extern ngx_uint_t      ngx_exiting;

extern sig_atomic_t    ngx_reap;//信号处理时，防止变量访问不会被中断，要求访问或改变在计算机的一条指令内完成，通常int类型的变量通常是原子访问；
extern sig_atomic_t    ngx_sigio;//
extern sig_atomic_t    ngx_sigalrm;
extern sig_atomic_t    ngx_quit;
extern sig_atomic_t    ngx_debug_quit;
extern sig_atomic_t    ngx_terminate;
extern sig_atomic_t    ngx_noaccept;
extern sig_atomic_t    ngx_reconfigure;
extern sig_atomic_t    ngx_reopen;
extern sig_atomic_t    ngx_change_binary;
#endif /* _NGX_PROCESS_H_INCLUDED_ */
