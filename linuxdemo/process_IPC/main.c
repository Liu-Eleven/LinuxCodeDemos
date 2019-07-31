#include "ngx_process.h"



ngx_uint_t    ngx_process;
static ngx_uint_t   ngx_show_help;
static char        *ngx_signal;
ngx_uint_t    ngx_process;

static int debug = 0;

static void ngx_show_version_info(void);
static ngx_int_t create_pidfile(const char * filename);
static ngx_int_t ngx_get_options(int argc, char *const *argv);
ngx_int_t ngx_signal_process(char *sig);


int main(int argc,char **argv){
	int cpu_num ;
	
	if (ngx_get_options(argc, argv) != NGX_OK) {
        return 1;
    }

	if (ngx_show_help) {
        ngx_show_version_info();
        return 0;
    }

	
	if (ngx_signal) {
			return ngx_signal_process(ngx_signal);
	}

	if (ngx_init_signals() != NGX_OK) {
        return 1;
    }

	if(create_pidfile(PID_FILE) != NGX_OK){
		fprintf(stderr,"Create pid file failed, Exit!\n");
		return 0;
	}

	ngx_process = NGX_PROCESS_MASTER;

    ngx_master_process_cycle();
	return 0;
}


static ngx_int_t ngx_get_options(int argc, char *const *argv)
{
    char     *p;
    ngx_int_t   i;

    for (i = 1; i < argc; i++) {

        p = (char *) argv[i];

        if (*p++ != '-') {
            fprintf(stderr, "invalid option: \"%s\"\n", argv[i]);
            return NGX_ERROR;
        }

        while (*p) {

            switch (*p++) {

            case '?':
            case 'h':
                ngx_show_help = 1;
                break;


            case 's':
                if (*p) {
                    ngx_signal = (char *) p;

                } else if (argv[++i]) {
                    ngx_signal = argv[i];

                } else {
                    fprintf(stderr, "option \"-s\" requires parameter\n");
                    return NGX_ERROR;
                }

                if (strcmp(ngx_signal, "stop") == 0
                    || strcmp(ngx_signal, "quit") == 0
                    || strcmp(ngx_signal, "reopen") == 0
                    || strcmp(ngx_signal, "reload") == 0)
                {
                    ngx_process = NGX_PROCESS_SIGNALLER;
                    goto next;
                }

                fprintf(stderr, "invalid option: \"-s %s\"\n", ngx_signal);
                return NGX_ERROR;

            default:
                fprintf(stderr, "invalid option: \"%c\"\n", *(p - 1));
                return NGX_ERROR;
            }
        }

    next:

        continue;
    }

    return NGX_OK;
}


static void ngx_show_version_info(void)
{

    fprintf(stdout,
            "Usage: nginx [-?hvVtTq] [-s signal] [-c filename] "
                         "[-p prefix] [-g directives]" NGX_LINEFEED
                         NGX_LINEFEED
            "Options:" NGX_LINEFEED
            "  -?,-h         : this help" NGX_LINEFEED
            "  -s signal     : send signal to a master process: "
                               "stop, quit, reopen, reload" NGX_LINEFEED
    );
}

static ngx_int_t create_pidfile(const char *name)
{
    
    int fd = -1;
    int i = 0, len = 0;
	char buf[64];
	pid_t  pid;

    fd = open(name, O_RDWR|O_TRUNC|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
    if(fd < 0)
    {
	    fprintf(stderr, "fopen %s failed, reason: %s. \nexit.\n",name,strerror(errno));
	    return NGX_ERROR;
    }

	pid = getpid();
	len = snprintf(buf,64,"%d\r\n",(int)pid);

	if(len < 0 || write(fd,buf,len) != len)
	{
		fprintf(stderr, "write to %s failed, reason: %s. \n", PID_FILE, strerror(errno));
		close(fd);
		return NGX_ERROR;
	}

	close(fd);

	return NGX_OK;

}

ngx_int_t
ngx_signal_process(char *sig)
{
    size_t           n;
    pid_t             pid;
    char            buf[NGX_INT64_LEN + 2];
	int fd = -1;
    

    if(debug) fprintf(stdout, "signal process started\n");

	buf[0]='\0';
	
	fd = open(PID_FILE, O_RDWR);
    if(fd < 0)
    {
	    fprintf(stderr, "fopen %s failed, reason: %s. \nexit.\n",PID_FILE,strerror(errno));
	    return NGX_ERROR;
    }


	if( (n = read(fd,buf,NGX_INT64_LEN+2)) < 0 )
	{
		fprintf(stderr, "read from %s failed, reason: %s. \n", PID_FILE, strerror(errno));
		return NGX_ERROR;
	}

    while (n-- && (buf[n] == '\r' || buf[n] == '\n')) { buf[n]='\0';/* void */ }

    pid = (pid_t)atoi(buf);

    if (pid <=0 ) {
        fprintf(stderr,
                      "invalid PID number \"%s\" in \"%s\"",buf, PID_FILE);
        return 1;
    }

    if(debug) fprintf(stdout, "get pid: %d\n",(int)pid);

    return ngx_os_signal_process(sig, pid);

}











