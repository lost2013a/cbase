#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>

/*系统的时间片大小，一般是10MS 或者 1MS*/
#define TICKS_PER_SEC 10 
//#define TICKS_PER_SEC ((1000*1000)/CLOCKS_PER_SEC) 

long getNowTime(void)
{
    long t = times(0);
	return t;
}

void app_delay_test(void) {
    long t0,t1; 
    int wcnt = 3;
    long max = 100/TICKS_PER_SEC + 1;
    long min = 100/TICKS_PER_SEC - 1;
    while(wcnt--) {
        t0 = getNowTime();
        usleep(100*1000);
        t1 = getNowTime();
        printf("t1 - t0 = %ld\n", t1- t0);
        if(min <= t1-t0 && t1-t0 <= max) {
            break;
        }
    }
    if(wcnt <= 0) {
        printf("app delay parm err\n");
        exit(0);
    }
}
 
void app_sleep(volatile long *ptcnt, long u_ms)
{
	*ptcnt= getNowTime()+ u_ms/TICKS_PER_SEC;
}

unsigned char app_pend_wake(long tcnt)
{  
    return (getNowTime()- tcnt > 0) ? 1: 0;
}

unsigned char app_capture_interval(long tcnt, long u_ms)
{    
    return (getNowTime()- tcnt > u_ms*TICKS_PER_SEC) ? 1: 0;
}

void app_update_interval(volatile long *ptcnt)
{
	*ptcnt = getNowTime();
}



