#include "ngx_process.h"
#include "ngx_channel.h"

static int debug = 1;

ngx_uint_t    ngx_process;
ngx_uint_t    ngx_worker;
ngx_pid_t     ngx_pid;


sig_atomic_t  ngx_reap;
sig_atomic_t  ngx_sigio;
sig_atomic_t  ngx_sigalrm;
sig_atomic_t  ngx_terminate;
sig_atomic_t  ngx_quit;
sig_atomic_t  ngx_debug_quit;
ngx_uint_t    ngx_exiting;
sig_atomic_t  ngx_reconfigure;
sig_atomic_t  ngx_reopen;

sig_atomic_t  ngx_change_binary;
ngx_pid_t     ngx_new_binary;
ngx_uint_t    ngx_inherited;
ngx_uint_t    ngx_daemonized;

sig_atomic_t  ngx_noaccept;
ngx_uint_t    ngx_noaccepting;
ngx_uint_t    ngx_restart;

static int worker_processes;


static u_char  master_process[] = "master process";



static void worker_process_cycle(void *data);
static void ngx_start_worker_processes( ngx_int_t n, ngx_int_t type);
void ngx_master_process_cycle();
pid_t spawn_process(ngx_spawn_proc_pt proc, void *data, char *name, int respawn); 
static ngx_uint_t ngx_reap_children();
static void ngx_master_process_exit();
static void ngx_pass_open_channel(ngx_channel_t *ch);
static void try_ngx_channel_handler(int fd);


void
ngx_master_process_cycle()
{
    char              *title;
    u_char            *p;
    size_t             size;
    ngx_int_t          i;
    ngx_uint_t         n, sigio;
    sigset_t           set;
    struct itimerval   itv;
    ngx_uint_t         live;
    int                cpu_num ;
    ngx_uint_t         delay;

    

    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGIO);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGHUP);//NGX_RECONFIGURE_SIGNAL
    sigaddset(&set, SIGUSR1);//NGX_REOPEN_SIGNAL
    //sigaddset(&set, ngx_signal_value(NGX_NOACCEPT_SIGNAL));//NGX_NOACCEPT_SIGNAL
    sigaddset(&set, SIGTERM);//NGX_TERMINATE_SIGNAL
    sigaddset(&set, SIGQUIT);//NGX_SHUTDOWN_SIGNAL
    sigaddset(&set, SIGUSR2);//NGX_CHANGEBIN_SIGNAL

    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        fprintf(stderr,
                      "sigprocmask() failed");
    }

    sigemptyset(&set);

    //获取CPU核数
    worker_processes = sysconf(_SC_NPROCESSORS_ONLN);
	worker_processes = 1;//worker_processes>0 ? worker_processes:1;
	
    ngx_start_worker_processes(worker_processes, NGX_PROCESS_RESPAWN);

    ngx_new_binary = 0;
    delay = 0;
    sigio = 0;
    live = 1;

    for ( ;; ) {
        if (delay) {
            if (ngx_sigalrm) {
                sigio = 0;
                delay *= 2;
                ngx_sigalrm = 0;
            }

            if(debug)fprintf(stdout,"termination cycle: %lu\n", delay);

            itv.it_interval.tv_sec = 0;
            itv.it_interval.tv_usec = 0;
            itv.it_value.tv_sec = delay / 1000;
            itv.it_value.tv_usec = (delay % 1000 ) * 1000;

            if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
                fprintf(stderr,"setitimer() failed,reason:%s\n",strerror(errno));
            }
        }

        if(debug)fprintf(stdout, "sigsuspend\n");

        sigsuspend(&set);
		
        //ngx_time_update();

        if(debug) fprintf(stdout,"wake up, sigio %lu\n", sigio);

        if (ngx_reap) {
            ngx_reap = 0;
            if(debug)fprintf(stdout, "reap children\n");

            live = ngx_reap_children();
        }

        if (!live && (ngx_terminate || ngx_quit)) {
            ngx_master_process_exit();
        }

        if (ngx_terminate) {
            if (delay == 0) {
                delay = 50;
            }

            if (sigio) {
                sigio--;
                continue;
            }

            sigio = worker_processes  /*no cache processes */;

            if (delay > 1000) {
                ngx_signal_worker_processes(SIGKILL);
            } else {
                ngx_signal_worker_processes(SIGTERM);//NGX_TERMINATE_SIGNAL
            }

            continue;
        }

        if (ngx_quit) {
            ngx_signal_worker_processes(SIGQUIT);//NGX_SHUTDOWN_SIGNAL

            //关闭网络监听
            continue;
        }

        if (ngx_reconfigure) {
            ngx_reconfigure = 0;

            if (ngx_new_binary) {
                ngx_start_worker_processes(worker_processes,
                                           NGX_PROCESS_RESPAWN);
                ngx_noaccepting = 0;

                continue;
            }

            if(debug) fprintf(stdout, "reconfiguring\n");

            /*cycle = ngx_init_cycle(cycle);
            if (cycle == NULL) {
                cycle = (ngx_cycle_t *) ngx_cycle;
                continue;
            }

            ngx_cycle = cycle;
            ccf = (ngx_core_conf_t *) ngx_get_conf(cycle->conf_ctx,
                                                   ngx_core_module);
            */
            ngx_start_worker_processes(worker_processes,
                                       NGX_PROCESS_JUST_RESPAWN);

            /* allow new processes to start */
            usleep(100000);

            live = 1;
            ngx_signal_worker_processes(SIGQUIT);//NGX_SHUTDOWN_SIGNAL
        }

        if (ngx_restart) {
            ngx_restart = 0;
            ngx_start_worker_processes(worker_processes,NGX_PROCESS_RESPAWN);
            live = 1;
        }

        if (ngx_reopen) {
            ngx_reopen = 0;
            if(debug) fprintf(stdout, "reopening logs\n");
            //ngx_reopen_files(cycle, ccf->user);
            ngx_signal_worker_processes(SIGUSR1);//NGX_REOPEN_SIGNAL
        }

        if (ngx_change_binary) {
            ngx_change_binary = 0;
            fprintf(stdout, "changing binary\n");
			//热切换
            //ngx_new_binary = ngx_exec_new_binary( ngx_argv);
        }

        if (ngx_noaccept) {
            ngx_noaccept = 0;
            ngx_noaccepting = 1;
            ngx_signal_worker_processes(SIGQUIT);//NGX_SHUTDOWN_SIGNAL
        }
    }
}




static void
ngx_start_worker_processes( ngx_int_t n, ngx_int_t type)
{
    ngx_int_t      i;
    ngx_channel_t  ch;

    if(debug) fprintf(stdout, "start worker processes %ld\n", n);

    memset(&ch, 0, sizeof(ngx_channel_t));

    ch.command = NGX_CMD_OPEN_CHANNEL;

    for (i = 0; i < n; i++) {

        ngx_spawn_process(worker_process_cycle,
                          (void *) (intptr_t) i, "worker process", type);

        ch.pid = ngx_processes[ngx_process_slot].pid;
        ch.slot = ngx_process_slot;
        ch.fd = ngx_processes[ngx_process_slot].channel[0];

        ngx_pass_open_channel(&ch);
    }
}



static void worker_prcoess_init(int worker){
    cpu_set_t cpu_affinity;
	ngx_int_t         n;
    ngx_uint_t        i;

    CPU_ZERO(&cpu_affinity);
    CPU_SET(worker % CPU_SETSIZE,&cpu_affinity);
    //sched_setaffinity
    if(sched_setaffinity(0,sizeof(cpu_set_t),&cpu_affinity) == -1){
        fprintf(stderr, "sched_setaffinity() failed\n");
    }

    sigset_t          set;
	sigemptyset(&set);

    if (sigprocmask(SIG_SETMASK, &set, NULL) == -1) {
        fprintf(stderr,
                      "sigprocmask() failed, reason: %s\n",strerror(errno));
    }

	
    for (n = 0; n < ngx_last_process; n++) {

        if (ngx_processes[n].pid == -1) {
            continue;
        }

        if (n == ngx_process_slot) {
            continue;
        }

        if (ngx_processes[n].channel[1] == -1) {
            continue;
        }

        if (close(ngx_processes[n].channel[1]) == -1) {
            fprintf(stderr,
                          "close() channel failed, reason: %s\n",strerror(errno));
        }
    }

    if (close(ngx_processes[ngx_process_slot].channel[0]) == -1) {
		fprintf(stderr,
                          "close() channel failed, reason: %s\n",strerror(errno));
    }
}

void worker_process_cycle(void *data){
     int worker = (intptr_t) data;
    //初始化

	ngx_process = NGX_PROCESS_WORKER;
    ngx_worker = worker;
	
    worker_prcoess_init(worker);

    
    for(;;){
		
	  try_ngx_channel_handler(ngx_channel);
	
      
       if (ngx_exiting) {
            //if (ngx_event_no_timers_left() == NGX_OK) {
                if(debug) fprintf(stdout, "child process[%d] exiting\n",getpid());
                    exit(0);//ngx_worker_process_exit(cycle); 
            //}
        }
        //ngx_process_events_and_timers(cycle);

        if (ngx_terminate) {
            if(debug) fprintf(stdout, "child process[%d] exiting. SIGTERM\n",getpid());
            exit(0);//ngx_worker_process_exit(cycle);
        }

        if (ngx_quit) {
            ngx_quit = 0;
            if(debug) fprintf(stdout,"gracefully shutting down\n");
            //ngx_setproctitle("worker process is shutting down");

            if (!ngx_exiting) {
                ngx_exiting = 1;
				/*优雅的关闭资源*/
                //ngx_set_shutdown_timer(cycle);
                //ngx_close_listening_sockets(cycle);
                //ngx_close_idle_connections(cycle);
            }
        }

        if (ngx_reopen) {
            ngx_reopen = 0;
            if(debug)fprintf(stdout, "child process[%d] reopening logs\n",getpid());
            //ngx_reopen_files(cycle, -1);
        }
      sleep(5);
      if(debug)fprintf(stdout,"work process[%d] cycle.\n",getpid());
    }
}


void  ngx_signal_worker_processes(int signo)
{
	ngx_int_t	   i;
	int       	   err;
	ngx_channel_t  ch;
	
	memset(&ch, 0, sizeof(ngx_channel_t));
	
	switch (signo) {
	
	case SIGQUIT://NGX_SHUTDOWN_SIGNAL
		ch.command = NGX_CMD_QUIT;
		break;
	
	case SIGTERM://NGX_TERMINATE_SIGNAL
		ch.command = NGX_CMD_TERMINATE;
		break;
	
	case SIGUSR1://NGX_REOPEN_SIGNAL
		ch.command = NGX_CMD_REOPEN;
		break;
	
	default:
		ch.command = 0;
	}
	
	ch.fd = -1;
	
	
	for (i = 0; i < ngx_last_process; i++) {
	
		if(debug)fprintf(stdout,"child: %ld %d e:%d t:%d d:%d r:%d j:%d\n",
						  i,
						  ngx_processes[i].pid,
						  ngx_processes[i].exiting,
						  ngx_processes[i].exited,
						  ngx_processes[i].detached,
						  ngx_processes[i].respawn,
						  ngx_processes[i].just_spawn);
	
		if (ngx_processes[i].detached || ngx_processes[i].pid == -1) {
			continue;
		}
	
		if (ngx_processes[i].just_spawn) {
			ngx_processes[i].just_spawn = 0;
			continue;
		}
	
		if (ngx_processes[i].exiting
			&& signo == SIGQUIT)     //NGX_SHUTDOWN_SIGNAL
		{
			continue;
		}
	
		if (ch.command) {
			if (ngx_write_channel(ngx_processes[i].channel[0],
									  &ch, sizeof(ngx_channel_t))
				== NGX_OK)
			{
				if (signo != SIGUSR1) { //NGX_REOPEN_SIGNAL
					ngx_processes[i].exiting = 1;
				}
	
				continue;
			}
		}
	
		if(debug) fprintf(stdout,
						   "kill (%d, %d)\n", ngx_processes[i].pid, signo);
	
		if (kill(ngx_processes[i].pid, signo) == -1) {
			err = errno;
			fprintf(stderr,"kill(%d, %d) failed\n", ngx_processes[i].pid, signo);
	
			if (err == ESRCH) {
				ngx_processes[i].exited = 1;
				ngx_processes[i].exiting = 0;
				ngx_reap = 1;
			}
	
			continue;
		}
	
		if (signo != SIGUSR1) {//NGX_REOPEN_SIGNAL
			ngx_processes[i].exiting = 1;
		}
	}
}



static ngx_uint_t
ngx_reap_children()
{
    ngx_int_t         i, n;
    ngx_uint_t        live;
    ngx_channel_t     ch;
    

    memset(&ch, 0, sizeof(ngx_channel_t));

    ch.command = NGX_CMD_CLOSE_CHANNEL;
    ch.fd = -1;

    live = 0;
    for (i = 0; i < ngx_last_process; i++) {

        if(debug) fprintf(stdout,
                       "child: %ld %d e:%d t:%d d:%d r:%d j:%d\n",
                       i,
                       (int)ngx_processes[i].pid,
                       ngx_processes[i].exiting,
                       ngx_processes[i].exited,
                       ngx_processes[i].detached,
                       ngx_processes[i].respawn,
                       ngx_processes[i].just_spawn);

        if (ngx_processes[i].pid == -1) {
            continue;
        }

        if (ngx_processes[i].exited) {

            if (!ngx_processes[i].detached) {
                ngx_close_channel(ngx_processes[i].channel);

                ngx_processes[i].channel[0] = -1;
                ngx_processes[i].channel[1] = -1;

                ch.pid = ngx_processes[i].pid;
                ch.slot = i;

                for (n = 0; n < ngx_last_process; n++) {
                    if (ngx_processes[n].exited
                        || ngx_processes[n].pid == -1
                        || ngx_processes[n].channel[0] == -1)
                    {
                        continue;
                    }

                    if(debug)fprintf(stdout,
                                   "pass close channel s:%d pid:%d to:%d\n",
                                   (int)ch.slot, (int)ch.pid, (int)ngx_processes[n].pid);

                    /* TODO: NGX_AGAIN */

                    ngx_write_channel(ngx_processes[n].channel[0],
                                      &ch, sizeof(ngx_channel_t));
                }
            }

            if (ngx_processes[i].respawn
                && !ngx_processes[i].exiting
                && !ngx_terminate
                && !ngx_quit)
            {
                if (ngx_spawn_process(ngx_processes[i].proc,
                                      ngx_processes[i].data,
                                      ngx_processes[i].name, i)
                    == NGX_INVALID_PID)
                {
                    fprintf(stderr,
                                  "could not respawn %s\n",
                                  ngx_processes[i].name);
                    continue;
                }


                ch.command = NGX_CMD_OPEN_CHANNEL;
                ch.pid = ngx_processes[ngx_process_slot].pid;
                ch.slot = ngx_process_slot;
                ch.fd = ngx_processes[ngx_process_slot].channel[0];

                ngx_pass_open_channel(&ch);

                live = 1;

                continue;
            }

            if (ngx_processes[i].pid == ngx_new_binary) {

                /*
                if (ngx_rename_file((char *) ccf->oldpid.data,
                                    (char *) ccf->pid.data)
                    == NGX_FILE_ERROR)
                {
                    ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
                                  ngx_rename_file_n " %s back to %s failed "
                                  "after the new binary process \"%s\" exited",
                                  ccf->oldpid.data, ccf->pid.data, ngx_argv[0]);
                }

                ngx_new_binary = 0;
                if (ngx_noaccepting) {
                    ngx_restart = 1;
                    ngx_noaccepting = 0;
                }*/
            }

            if (i == ngx_last_process - 1) {
                ngx_last_process--;

            } else {
                ngx_processes[i].pid = -1;
            }

        } else if (ngx_processes[i].exiting || !ngx_processes[i].detached) {
            live = 1;
        }
    }

    return live;
}


static void
ngx_master_process_exit()
{
    ngx_uint_t  i;

    //ngx_delete_pidfile(cycle);
    if(unlink(PID_FILE)) fprintf(stderr,"unlink %s failed, reason: %s\n", PID_FILE, strerror(errno));

    fprintf(stdout, "exit\n");

   //释放资源
   
    exit(0);
}

static void
ngx_pass_open_channel(ngx_channel_t *ch)
{
    ngx_int_t  i;

    for (i = 0; i < ngx_last_process; i++) {

        if (i == ngx_process_slot
            || ngx_processes[i].pid == -1
            || ngx_processes[i].channel[0] == -1)
        {
            continue;
        }

        if(debug) fprintf(stdout,
                      "pass channel s:%ld pid:%d fd:%d to s:%ld pid:%d fd:%d\n",
                      ch->slot, (int)ch->pid, ch->fd,
                      i, (int)ngx_processes[i].pid,
                      (int)ngx_processes[i].channel[0]);

        /* TODO: NGX_AGAIN */

        ngx_write_channel(ngx_processes[i].channel[0],
                          ch, sizeof(ngx_channel_t));
    }
}


static void try_ngx_channel_handler(int fd)
{
    ngx_int_t          n;
    ngx_channel_t      ch;
   
	//if(debug) fprintf(stdout,"channel handler\n");
	
    for ( ;; ) {

        n = ngx_read_channel(fd, &ch, sizeof(ngx_channel_t));

        if (n == NGX_ERROR) {
		/*
            if (ngx_event_flags & NGX_USE_EPOLL_EVENT) {
                ngx_del_conn(c, 0);
            }
		
            ngx_close_connection(c);*/
            
            return;
        }
		
		
        if (n == NGX_AGAIN) {
            return;
        }

        if(debug) fprintf(stdout,"channel command: %lu\n", ch.command);

        switch (ch.command) {

        case NGX_CMD_QUIT:
            ngx_quit = 1;
            break;

        case NGX_CMD_TERMINATE:
            ngx_terminate = 1;
            break;

        case NGX_CMD_REOPEN:
            ngx_reopen = 1;
            break;

        case NGX_CMD_OPEN_CHANNEL:

            if(debug)fprintf(stdout,
                           "get channel s:%d pid:%d fd:%d\n",(int)ch.slot, (int)ch.pid, (int)ch.fd);

            ngx_processes[ch.slot].pid = ch.pid;
            ngx_processes[ch.slot].channel[0] = ch.fd;
            break;

        case NGX_CMD_CLOSE_CHANNEL:

            if(debug)fprintf(stdout,
                           "close channel s:%d pid:%d our:%d fd:%d\n",
                           (int)ch.slot, (int)ch.pid, (int)ngx_processes[ch.slot].pid,
                           (int)ngx_processes[ch.slot].channel[0]);

            if (close(ngx_processes[ch.slot].channel[0]) == -1) {
                fprintf(stdout,"close() channel failed\n");
            }

            ngx_processes[ch.slot].channel[0] = -1;
            break;
        }
    }
}












