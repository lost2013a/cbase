#include <sys/times.h>
/*系统的时间片大小，一般是10MS 或者 1MS*/
#define TICKS_PER_SEC 10 

static unsigned int console_lv = 0xffff;
//static unsigned int console_lv = 0;
void console_lv_set(unsigned char ch) {
	if(ch >= 32)
		return;
	console_lv |= 0x1<<ch;
}
void console_lv_clear(unsigned char ch) {
	if(ch >= 32)
		return;
	console_lv &= ~(0x1<<ch);
}
unsigned int console_lv_get(unsigned char ch) {
	if(ch >= 32)
		return 0;
	return console_lv&(0x1<<ch);
}


static long getNowTime(void) {
    long t = times(0);
	return t;
}
void app_sleep(volatile long *ptcnt, long u_ms) {
	*ptcnt= getNowTime()+ u_ms/TICKS_PER_SEC;
}
unsigned char app_pend_wake(long tcnt) {  
    return (getNowTime()- tcnt > 0) ? 1: 0;
}
unsigned char app_capture_interval(long tcnt, long u_ms) {    
    return (getNowTime()- tcnt > u_ms*TICKS_PER_SEC) ? 1: 0;
}
void app_update_interval(volatile long *ptcnt) {
	*ptcnt = getNowTime();
}


