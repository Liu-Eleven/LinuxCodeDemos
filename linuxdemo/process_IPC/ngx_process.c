
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */



#include "ngx_process.h"
#include "ngx_channel.h"


typedef struct {
    int     signo;
    char   *signame;
    char   *name;
    void  (*handler)(int signo);
} ngx_signal_t;

static int debug = 1;

static void ngx_execute_proc( void *data);
static void ngx_signal_handler(int signo);
static void ngx_process_get_status(void);



int set_nonblock(int fd)
{
    int fl = fcntl(fd,F_GETFL);
    return fcntl(fd,F_SETFL,fl | O_NONBLOCK);
}

//static void ngx_unlock_mutexes(ngx_pid_t pid);


int              ngx_argc;
char           **ngx_argv;
char           **ngx_os_argv;

ngx_int_t        ngx_process_slot;
int              ngx_channel;
ngx_int_t        ngx_last_process;
ngx_process_t    ngx_processes[NGX_MAX_PROCESSES];


ngx_signal_t  signals[] = {
    { SIGHUP,             //RECONFIGURE
      "SIGHUP",
      "reload",
      ngx_signal_handler },

    { SIGUSR1, //REOPEN
      "SIGUSR1" ,
      "reopen",
      ngx_signal_handler },

    { SIGWINCH,
      "SIGWINCH",
      "",
      ngx_signal_handler },

    { SIGTERM,
      "SIGTERM" ,
      "stop",
      ngx_signal_handler },

    { SIGQUIT,
      "SIGQUIT",
      "quit",
      ngx_signal_handler },

    { SIGUSR2,
      "SIGUSR2",
      "",
      ngx_signal_handler },

    { SIGALRM, "SIGALRM", "", ngx_signal_handler },

    { SIGINT, "SIGINT", "", ngx_signal_handler },

    { SIGIO, "SIGIO", "", ngx_signal_handler },

    { SIGCHLD, "SIGCHLD", "", ngx_signal_handler },

    { SIGSYS, "SIGSYS, SIG_IGN", "", SIG_IGN },

    { SIGPIPE, "SIGPIPE, SIG_IGN", "", SIG_IGN },

    { 0, NULL, "", NULL }
};


ngx_pid_t
ngx_spawn_process( ngx_spawn_proc_pt proc, void *data,
    char *name, ngx_int_t respawn)
{
    u_long     on;
    ngx_pid_t  pid;
    ngx_int_t  s;

    if (respawn >= 0) {
        s = respawn;

    } else {
        for (s = 0; s < ngx_last_process; s++) {
            if (ngx_processes[s].pid == -1) {
                break;
            }
        }

		if (s == NGX_MAX_PROCESSES) {
					fprintf(stderr,"no more than %d processes can be spawned\n",NGX_MAX_PROCESSES);
					return NGX_INVALID_PID;
				}
			}
		
			
			 if (respawn != NGX_PROCESS_DETACHED) {
		
				/* Solaris 9 still has no AF_LOCAL */
		
				if (socketpair(AF_UNIX, SOCK_STREAM, 0, ngx_processes[s].channel) == -1)
				{
					fprintf(stderr,
								  "socketpair() failed while spawning \"%s\"\n", name);
					return NGX_INVALID_PID;
				}
		
				fprintf(stdout,"channel %d:%d\n", ngx_processes[s].channel[0], ngx_processes[s].channel[1]);
		
				if (set_nonblock(ngx_processes[s].channel[0]) == -1) {
					fprintf(stderr,
								   "set nonblock failed while spawning \"%s\"\n",name);
					ngx_close_channel(ngx_processes[s].channel);
					return NGX_INVALID_PID;
				}
		
				if (set_nonblock(ngx_processes[s].channel[1]) == -1) {
					fprintf(stderr,
								   "set nonblock failed while spawning \"%s\"\n",name);
					ngx_close_channel(ngx_processes[s].channel);
					return NGX_INVALID_PID;
				}
		
				on = 1;
				if (ioctl(ngx_processes[s].channel[0], FIOASYNC, &on) == -1) {
					fprintf(stderr,
								  "ioctl(FIOASYNC) failed while spawning \"%s\"\n", name);
					ngx_close_channel(ngx_processes[s].channel);
					return NGX_INVALID_PID;
				}
		
				if (fcntl(ngx_processes[s].channel[0], F_SETOWN, ngx_pid) == -1) {
					fprintf(stderr,
								  "fcntl(F_SETOWN) failed while spawning \"%s\"\n", name);
					ngx_close_channel(ngx_processes[s].channel);
					return NGX_INVALID_PID;
				}
		
				if (fcntl(ngx_processes[s].channel[0], F_SETFD, FD_CLOEXEC) == -1) {
					fprintf(stderr,
								  "fcntl(FD_CLOEXEC) failed while spawning \"%s\"\n",
								   name);
					ngx_close_channel(ngx_processes[s].channel);
					return NGX_INVALID_PID;
				}
		
				if (fcntl(ngx_processes[s].channel[1], F_SETFD, FD_CLOEXEC) == -1) {
					fprintf(stderr,
								  "fcntl(FD_CLOEXEC) failed while spawning \"%s\"\n",
								   name);
					ngx_close_channel(ngx_processes[s].channel);
					return NGX_INVALID_PID;
				}

        ngx_channel = ngx_processes[s].channel[1];

    } else {
        ngx_processes[s].channel[0] = -1;
        ngx_processes[s].channel[1] = -1;
    }

    ngx_process_slot = s;


    pid = fork();

    switch (pid) {

    case -1:
        fprintf(stderr,"fork() failed while spawning \"%s\"\n",name);
        ngx_close_channel(ngx_processes[s].channel);
        return NGX_INVALID_PID;

    case 0:
        ngx_pid = getpid();
        proc(data);
        break;

    default:
        break;
    }

    if(debug) fprintf(stdout, "start %s %d\n", name, (int)pid);

    ngx_processes[s].pid = pid;
    ngx_processes[s].exited = 0;

    if (respawn >= 0) {
        return pid;
    }

    ngx_processes[s].proc = proc;
    ngx_processes[s].data = data;
    ngx_processes[s].name = name;
    ngx_processes[s].exiting = 0;

    switch (respawn) {

    case NGX_PROCESS_NORESPAWN:
        ngx_processes[s].respawn = 0;
        ngx_processes[s].just_spawn = 0;
        ngx_processes[s].detached = 0;
        break;

    case NGX_PROCESS_JUST_SPAWN:
        ngx_processes[s].respawn = 0;
        ngx_processes[s].just_spawn = 1;
        ngx_processes[s].detached = 0;
        break;

    case NGX_PROCESS_RESPAWN:
        ngx_processes[s].respawn = 1;
        ngx_processes[s].just_spawn = 0;
        ngx_processes[s].detached = 0;
        break;

    case NGX_PROCESS_JUST_RESPAWN:
        ngx_processes[s].respawn = 1;
        ngx_processes[s].just_spawn = 1;
        ngx_processes[s].detached = 0;
        break;

    case NGX_PROCESS_DETACHED:
        ngx_processes[s].respawn = 0;
        ngx_processes[s].just_spawn = 0;
        ngx_processes[s].detached = 1;
        break;
    }

    if (s == ngx_last_process) {
        ngx_last_process++;
    }

    return pid;
}


ngx_pid_t
ngx_execute(ngx_exec_ctx_t *ctx)
{
    return ngx_spawn_process( ngx_execute_proc, ctx, ctx->name,
                             NGX_PROCESS_DETACHED);
}


static void
ngx_execute_proc(void *data)
{
    ngx_exec_ctx_t  *ctx = data;

    if (execve(ctx->path, ctx->argv, ctx->envp) == -1) {
        fprintf(stderr,
                      "execve() failed while executing %s \"%s\"\n",
                      ctx->name, ctx->path);
    }

    exit(1);
}


ngx_int_t
ngx_init_signals()
{
    ngx_signal_t      *sig;
    struct sigaction   sa;

    for (sig = signals; sig->signo != 0; sig++) {
		if(debug) fprintf(stdout,"set up signal: %s %s %d\n",sig->signame,sig->name,sig->signo);
        memset(&sa,0,sizeof(struct sigaction));
        sa.sa_handler = sig->handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(sig->signo, &sa, NULL) == -1) {

            fprintf(stderr,"sigaction(%s) failed,reason:%s\n", sig->signame,strerror(errno));
            return NGX_ERROR;
        }
    }

    return NGX_OK;
}


static void
ngx_signal_handler(int signo)
{
    char            *action;
    ngx_int_t        ignore;
    int              err;
    ngx_signal_t    *sig;

    ignore = 0;

    err = errno;

	if(debug) printf("catch a signal %d.\n",signo);
	
    for (sig = signals; sig->signo != 0; sig++) {
        if (sig->signo == signo) {
            break;
        }
    }

    action = "";

    switch (ngx_process) {

    case NGX_PROCESS_MASTER:
    
        switch (signo) {

        case SIGQUIT:
            ngx_quit = 1;
            action = ", shutting down";
            break;

        case SIGTERM:
        case SIGINT:
            ngx_terminate = 1;
            action = ", exiting";
            break;

        case SIGWINCH:
            if (ngx_daemonized) {
                ngx_noaccept = 1;
                action = ", stop accepting connections";
            }
            break;

        case SIGHUP:
            ngx_reconfigure = 1;
            action = ", reconfiguring";
            break;

        case SIGUSR1://NGX_REOPEN_SIGNAL
            ngx_reopen = 1;
            action = ", reopening logs";
            break;

        case SIGUSR2:  //NGX_CHANGEBIN_SIGNAL
            if (getppid() > 1 || ngx_new_binary > 0) {

                /*
                 * Ignore the signal in the new binary if its parent is
                 * not the init process, i.e. the old binary's process
                 * is still running.  Or ignore the signal in the old binary's
                 * process if the new binary's process is already running.
                 */

                action = ", ignoring";
                ignore = 1;
                break;
            }

            ngx_change_binary = 1;
            action = ", changing binary";
            break;

        case SIGALRM:
            ngx_sigalrm = 1;
            break;

        case SIGIO:
            ngx_sigio = 1;
            break;

        case SIGCHLD:
            ngx_reap = 1;
            break;
        }

        break;

    case NGX_PROCESS_WORKER:
    
        switch (signo) {

        case SIGWINCH:
            /*if (!ngx_daemonized) {
                break;
            }*/
            //ngx_debug_quit = 1;
            break;
        case SIGQUIT:
            ngx_quit = 1;
            action = ", shutting down";
            break;

        case SIGTERM:
        case SIGINT:
            ngx_terminate = 1;
            action = ", exiting";
            break;

        case SIGUSR1:
            ngx_reopen = 1;
            action = ", reopening logs";
            break;

        case SIGHUP: //NGX_RECONFIGURE_SIGNAL
        case SIGUSR2: //NGX_CHANGEBIN_SIGNAL
        case SIGIO:
            action = ", ignoring";
            break;
        }

        break;
    }

    fprintf(stdout,
                  "signal %d (%s) received%s\n", signo, sig->signame, action);

    if (ignore) {
        fprintf(stderr,
                      "the changing binary signal is ignored: \n"
                      "you should shutdown or terminate \n"
                      "before either old or new binary's process\n");
    }

    if (signo == SIGCHLD) {
        ngx_process_get_status();
    }
	errno = err;
    //ngx_set_errno(err);
}


static void
ngx_process_get_status(void)
{
    int              status;
    char            *process;
    ngx_pid_t        pid;
    int              err;
    ngx_int_t        i;
    ngx_uint_t       one;

    one = 0;

    for ( ;; ) {
        pid = waitpid(-1, &status, WNOHANG);

        if (pid == 0) {
            return;
        }

        if (pid == -1) {
            err = errno;

            if (err == EINTR) {
                continue;
            }

            if (err == ECHILD && one) {
                return;
            }

            /*
             * Solaris always calls the signal handler for each exited process
             * despite waitpid() may be already called for this process.
             *
             * When several processes exit at the same time FreeBSD may
             * erroneously call the signal handler for exited process
             * despite waitpid() may be already called for this process.
             */

            if (err == ECHILD) {
                fprintf(stderr,"waitpid() failed\n");
                return;
            }

                fprintf(stderr,"waitpid() failed\n");
            return;
        }


        one = 1;
        process = "unknown process";

        for (i = 0; i < ngx_last_process; i++) {
            if (ngx_processes[i].pid == pid) {
                ngx_processes[i].status = status;
                ngx_processes[i].exited = 1;
                process = ngx_processes[i].name;
                break;
            }
        }

        if (WTERMSIG(status)) {
            fprintf(stderr,"%s %P exited on signal %d\n",
                          process, pid, WTERMSIG(status));

        } else {
            fprintf(stderr,"%s %d exited with code %d\n",
                          process, pid, WEXITSTATUS(status));
        }

        if (WEXITSTATUS(status) == 2 && ngx_processes[i].respawn) {//WEXITSTATUS(status) 当WIFEXITED返回非零值时，我们可以用这个宏来提取子进程的返回值，如果子进程调用exit(5)退出，WEXITSTATUS(status)就会返回5；如果子进程调用exit(7)，WEXITSTATUS(status)就会返回7。请注意，如果进程不是正常退出的，也就是说，WIFEXITED返回0，这个值就毫无意义。
            fprintf(stderr,"%s %P exited with fatal code %d "
                          "and cannot be respawned\n",
                          process, pid, WEXITSTATUS(status));
            ngx_processes[i].respawn = 0;
        }

        //ngx_unlock_mutexes(pid);
    }
}



ngx_int_t
ngx_os_signal_process(char *name, ngx_pid_t pid)
{
    ngx_signal_t  *sig;

    if(debug) fprintf(stdout,"want to kill %d by signal %s ...\n",pid,name?name:"NULL");
    for (sig = signals; sig->signo != 0; sig++) {
        if (strcmp(name, sig->name) == 0) {
			if(debug) fprintf(stdout,"kill %d by signal %s\n",(int)pid,sig->name);
            if (kill(pid, sig->signo) != -1) {
                return 0;
            }

            fprintf(stderr,
                          "kill(%d, %d) failed\n", (int)pid, sig->signo);
        }
    }

    return 1;
}
