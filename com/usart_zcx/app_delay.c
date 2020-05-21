#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "app_delay.h"

volatile unsigned int tcnt=0;


static unsigned int ticktime_get(void)
{
#define TICKS_PER_SEC 10
#include<sys/times.h>
	unsigned int ticks = (unsigned int)times( NULL);
	return (unsigned int)ticks*TICKS_PER_SEC;
}

unsigned int app_get_systime(void)
{
	return ticktime_get();
}

#define getNowTime  app_get_systime
#define K_MS 		1u

void app_sleep(volatile unsigned int *ptcnt, unsigned int u_ms)
{
	*ptcnt= getNowTime()+ u_ms/K_MS;
}

unsigned char app_pend_wake(unsigned int tcnt)
{
	unsigned int off = getNowTime()- tcnt;
	if(off>0 && off<0xff000000)
		return 1;
	else
		return 0;
}

unsigned char app_capture_interval(volatile unsigned int *ptcnt, unsigned int u_ms)
{
	unsigned int off = getNowTime()- *ptcnt;
	if(off>u_ms && off<0xff000000)
		return 1;
	else
		return 0;
}

void app_update_interval(volatile unsigned int *ptcnt)
{
	*ptcnt = getNowTime();
}


int app_read_interval(volatile unsigned int ptcnt)
{
	return getNowTime()- ptcnt;
}

#if 0

int main(int argc, char *argv[]) 
{	

	volatile unsigned int tcnt=0;
	while(1)
	{
//		if(app_pend_wake(tcnt)){
//			app_sleep(&tcnt, 1000);
//			printf("wake\r\n");
//		}
		
		if(app_capture_interval(&tcnt, 2000)){
			printf("wake\r\n");	
			app_update_interval(&tcnt);
		}
		usleep(10*1000);
		
	}


	return 0;
}
#endif
