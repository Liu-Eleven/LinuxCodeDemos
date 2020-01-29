#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <time.h>
#include <unistd.h>
#include <ucontext.h>

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

	// X86
	//for (i = 0; i < NGREG; i++)
	//	printf("reg[%02d]       = 0x%08x\n", i, ptrUC->uc_mcontext.gregs[i]);

	// ARM
	printf("arm_r0 = 0x%08x\n", ptrUC->uc_mcontext.arm_r0);
	printf("arm_r1 = 0x%08x\n", ptrUC->uc_mcontext.arm_r1);
	printf("arm_r2 = 0x%08x\n", ptrUC->uc_mcontext.arm_r2);
	printf("arm_r3 = 0x%08x\n", ptrUC->uc_mcontext.arm_r3);
	printf("arm_r4 = 0x%08x\n", ptrUC->uc_mcontext.arm_r4);
	printf("arm_r5 = 0x%08x\n", ptrUC->uc_mcontext.arm_r5);
	printf("arm_r6 = 0x%08x\n", ptrUC->uc_mcontext.arm_r6);
	printf("arm_r7 = 0x%08x\n", ptrUC->uc_mcontext.arm_r7);
	printf("arm_r8 = 0x%08x\n", ptrUC->uc_mcontext.arm_r8);
	printf("arm_r9 = 0x%08x\n", ptrUC->uc_mcontext.arm_r9);
	printf("arm_r10 = 0x%08x\n", ptrUC->uc_mcontext.arm_r10);
	printf("arm_fp = 0x%08x\n", ptrUC->uc_mcontext.arm_fp);
	printf("arm_ip = 0x%08x\n", ptrUC->uc_mcontext.arm_ip);
	printf("arm_sp = 0x%08x\n", ptrUC->uc_mcontext.arm_sp);
	printf("arm_lr = 0x%08x\n", ptrUC->uc_mcontext.arm_lr);
	printf("arm_pc = 0x%08x\n", ptrUC->uc_mcontext.arm_pc);

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

	// X86
	//for (int i = 0; i < NGREG; i++)
	//	fprintf(f_err, "reg[%02d]       = 0x%08x\n", i, ptrUC->uc_mcontext.gregs[i]);

	// ARM
	fprintf(f_err, "arm_r0 = 0x%08x\n", ptrUC->uc_mcontext.arm_r0);
	fprintf(f_err, "arm_r1 = 0x%08x\n", ptrUC->uc_mcontext.arm_r1);
	fprintf(f_err, "arm_r2 = 0x%08x\n", ptrUC->uc_mcontext.arm_r2);
	fprintf(f_err, "arm_r3 = 0x%08x\n", ptrUC->uc_mcontext.arm_r3);
	fprintf(f_err, "arm_r4 = 0x%08x\n", ptrUC->uc_mcontext.arm_r4);
	fprintf(f_err, "arm_r5 = 0x%08x\n", ptrUC->uc_mcontext.arm_r5);
	fprintf(f_err, "arm_r6 = 0x%08x\n", ptrUC->uc_mcontext.arm_r6);
	fprintf(f_err, "arm_r7 = 0x%08x\n", ptrUC->uc_mcontext.arm_r7);
	fprintf(f_err, "arm_r8 = 0x%08x\n", ptrUC->uc_mcontext.arm_r8);
	fprintf(f_err, "arm_r9 = 0x%08x\n", ptrUC->uc_mcontext.arm_r9);
	fprintf(f_err, "arm_r10 = 0x%08x\n", ptrUC->uc_mcontext.arm_r10);
	fprintf(f_err, "arm_fp = 0x%08x\n", ptrUC->uc_mcontext.arm_fp);
	fprintf(f_err, "arm_ip = 0x%08x\n", ptrUC->uc_mcontext.arm_ip);
	fprintf(f_err, "arm_sp = 0x%08x\n", ptrUC->uc_mcontext.arm_sp);
	fprintf(f_err, "arm_lr = 0x%08x\n", ptrUC->uc_mcontext.arm_lr);
	fprintf(f_err, "arm_pc = 0x%08x\n", ptrUC->uc_mcontext.arm_pc);

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
