#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

#include "tpool.h"
#include <time.h>


#define gettid() syscall(__NR_gettid)

//#define run_task_time()		usleep(120*10)
//#define add_task_time() 	usleep(100*10)
#define run_task_time()		usleep(10*1000)
#define add_task_time() 	

static unsigned int run_t;

void *func(void *arg)
{
	
	//unsigned int tid= gettid();
	//if(i != (unsigned int)arg)
	{
		//printf("run[#%02d,%02d] tid:%d, t-p:%d\n", (unsigned int)arg, run_t, tid, tid- getpid());
	}
	run_t++;
	run_task_time();
	return NULL;
}

unsigned int app_get_systime(void)
{	
#include <sys/time.h>
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned int)(tv.tv_sec*1000 + tv.tv_usec/1000);
}


int main(int arg, char **argv)
{
	unsigned int t=0;
	if (tpool_create(100) != 0) {
		printf("tpool_create failed\n");
		exit(1);
	}

	int i;
	t= app_get_systime();
	for (i = 0; i < 10000; ++i) {
		tpool_add_work(func, (void*)i);
		add_task_time();
	}
	t= app_get_systime()-t;
	printf("e: [%02d], time: %d.%d, run: %d\n", (run_t*100)/(t/10),t/1000,t%1000, run_t);
	tpool_destroy();
	return 0;
}
