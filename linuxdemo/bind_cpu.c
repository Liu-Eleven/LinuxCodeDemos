
#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/syscall.h>

#define __USE_GNU
#include <sched.h>

int bind_processor(int num) {
	pid_t self_id = syscall(__NR_gettid);
	
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(self_id % num, &mask);
	
	sched_setaffinity(0, sizeof(mask), &mask);
	
	for(;;);
}




int main() {
	int num = sysconf(_SC_NPROCESSORS_CONF);
	int i = 0;
	
	pid_t pid = 0;
	for(i=0; i<num/2; i++) {
		pid = fork();
		if(pid <= (pid_t)0) {
			usleep(1);
			break;
		}
	}
	
	if(pid > 0) {
		getchar();
	}

	bind_processor(num);
}


