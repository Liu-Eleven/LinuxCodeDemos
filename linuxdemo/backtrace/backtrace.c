#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <time.h>
#include <unistd.h>

// compile
// gcc -o test backtrace.c
// gcc -o test -g -rdynamic backtrace.c
//  
// objdump -d test > test.s
//
// ./test(+0xcdd) [0x7f0893200cdd]
// addr2line 0xcdd -e ./test -f

//#define PRINT_DEBUG

static void print_reason(int sig, siginfo_t * info, void *secret)
{
	void *array[10];
	size_t size;
	char file[64], buffer[1032];
	pid_t pid;
	ucontext_t* ptrUC = (ucontext_t*)secret;

	char **strings;
	size_t i;
	size = backtrace(array, 10);
	strings = backtrace_symbols(array, size);

	//动态链接库的映射地址是动态的，需要将maps文件打印出来
	pid = getpid();
	snprintf(file, sizeof(file), "/proc/%d/maps", pid);
	FILE *fp = fopen(file, "r");

#ifdef PRINT_DEBUG   
	printf("file = %s, line = %d, func = %s.\n", __FILE__, __LINE__, __FUNCTION__);
	printf("Obtained %zd stack frames.\n", size);
	for (i = 0; i < size; i++)
		printf("%s\n", strings[i]);

	if (NULL != fp)
	{
		while (fgets(buffer, 1024, fp))
		{
			fputs(buffer, stdout);
		}
		printf("Read MAPS file success!\n");
	}
	else
	{
		printf("Read MAPS file fail!\n");
	}

	for (i = 0; i < NGREG; i++)
		printf("reg[%02d]       = 0x%08x\n", i, ptrUC->uc_mcontext.gregs[i]);

#else
	FILE *f_err = fopen("err.log", "a+");

	fprintf(f_err, "file = %s, line = %d, func = %s.\n", __FILE__, __LINE__, __FUNCTION__);
	fprintf(f_err, "Obtained %zd stack frames.\n", size);
	for (i = 0; i < size; i++)
		fprintf(f_err, "%s\n", strings[i]);

	if (NULL != fp)
	{
		while (fgets(buffer, 1024, fp))
		{
			fputs(buffer, f_err);
		}
		fprintf(f_err, "Read MAPS file success!\n");
	}
	else
	{
		fprintf(f_err, "Read MAPS file fail!\n");
	}

	for (int i = 0; i < NGREG; i++)
		fprintf(f_err, "reg[%02d]       = 0x%08x\n", i, ptrUC->uc_mcontext.gregs[i]);

	fclose(f_err);
#endif

	free(strings);
	fclose(fp);
	exit(0);
}

void func()
{
	*((volatile char *)0x0) = 0x9999;
}

int main(int argc, char *argv[])
{
	//段错误捕获
	struct sigaction myAction;
	myAction.sa_sigaction = print_reason;
	sigemptyset(&myAction.sa_mask);
	myAction.sa_flags = SA_RESTART | SA_SIGINFO;
	sigaction(SIGSEGV, &myAction, NULL);
	sigaction(SIGUSR1, &myAction, NULL);
	sigaction(SIGFPE, &myAction, NULL);
	sigaction(SIGILL, &myAction, NULL);
	sigaction(SIGBUS, &myAction, NULL);
	sigaction(SIGABRT, &myAction, NULL);
	sigaction(SIGSYS, &myAction, NULL);
	
	printf("---------< 1 >-------------!\n");
	
	func();
	
	printf("---------< 2 >-------------!\n");

	return 0;
}
