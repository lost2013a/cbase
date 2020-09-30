#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "main.h"


static int display(void) {
    myarg(rcfg->shutdown_wcnt);
    return 0;
}

int pthread_create(pthread_t *tidp, const pthread_attr_t *attr, void *(*start_rtn)(void*),void *arg);

void task_creat(void* (*func)(void*)) {
    pthread_t tid;
    pthread_create(&tid, 0, func, 0);
}

void *task1(void *arg) {
    while(1) {
        sleep(3);
    }
}

void loop_systask(void)
{
    static volatile long tick0 = 0;
    while(1) {
        if (app_pend_wake(tick0)) {
            handle_savecfg();
            app_sleep(&tick0, 2000);
        }
        usleep(100*1000);
    }
}

int main(void) 
{   
    const unsigned char id[6]={1,2,3,4,5,6};
    display();
    CFGSET_SHUTDOWN_WCNT(30);
    CFGSET_LOGIC_ADDR(id, 6);
    display();
    task_creat(task1);
    loop_systask();
    return 0;
}











