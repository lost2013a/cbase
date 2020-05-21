#ifndef __APP_DELAY_H
#define __APP_DELAY_H
#include <sys/time.h>

unsigned int app_get_systime(void);

void app_sleep(volatile unsigned int *ptcnt, unsigned int u_ms);
unsigned char app_pend_wake(unsigned int tcnt);

unsigned char app_capture_interval(volatile unsigned int *ptcnt, unsigned int u_ms);
void app_update_interval(volatile unsigned int *ptcnt);


#endif

