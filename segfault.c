#define _GNU_SOURCE
#include <ucontext.h>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>


static void _signal_handler(int sig,siginfo_t *info,void *ctx)
{
#include <ucontext.h>
#include <execinfo.h>
    ucontext_t *context=(ucontext_t*)ctx;
    void *p_buf[256];
    char**p_strs;
    int i,nptrs;
    printf("signal=%d memory location=%p\n",sig,info->si_addr);
    nptrs = backtrace(p_buf,256);
    p_strs = backtrace_symbols(p_buf,nptrs);
    if(p_strs == NULL){
        perror("symbols err");
        exit(-1);
    }
    for(i=0;i<nptrs;i++){
        printf(" [%02d] %s\n",i,p_strs[i]);
    }
    free(p_strs);
}

void set_hexdump(void) {
	struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_sigaction = _signal_handler;
	action.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV,&action,NULL);
}





static void test3(int*p)
{
	*p =1 ;
}
static void test2(int*p)
{
	test3(p);
}
static void test1(int*p)
{
	test2(p);
}
int main(int argc,char*argv[])
{
	int *p=0x12345678;
	struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_sigaction = _signal_handler;
	action.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV,&action,NULL);
	test1(p);
	return 0;
}
