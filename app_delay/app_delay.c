#include <sys/times.h>

/*系统的时间片大小，一般是10MS 或者 1MS*/
#define TICKS_PER_SEC 10 

static long getNowTime(void)
{
    long t = times(0);
	return t;
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


int main(int argc, char *argv[]) 
{	
	volatile long tcnt=0;
	while(1)
	{
		if(app_pend_wake(tcnt)){
			app_sleep(&tcnt, 1000);
			printf("wake\r\n");
		}
	}
	return 0;
}

