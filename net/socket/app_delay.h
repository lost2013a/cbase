#ifndef __APP_DELAY_H
#define __APP_DELAY_H


void app_delay_test(void);
long getNowTime(void);

void app_sleep(volatile long *ptcnt, long u_ms);
unsigned char app_pend_wake(long tcnt);

unsigned char app_capture_interval(long tcnt, long u_ms);
void app_update_interval(volatile long *ptcnt);


#endif

