#ifndef __SHARE_H
#define __SHARE_H
#include <sys/time.h>


#define pr_fmt(fmt) ""fmt
#define console_cond(cond, fmt, args...)\
do {\
    if(cond)\
        printf(pr_fmt(fmt), ##args);\
}while(0)
#define console_log(ch, fmt, args...) console_cond(console_lv_get(ch), fmt, ##args) 

#define ebmdbg(fmt...)			console_log(0,##fmt) 
#define ebmdbg_rec(fmt...)		console_log(1,##fmt) 
#define ebmdbg_command(fmt...)	console_log(2,##fmt) 
#define ebmdbg_board(fmt...)	console_log(3,##fmt) 
#define ebmdbg_panel(fmt...)	console_log(4,##fmt) 

#define myarg(x) printf("%s = %d\n", #x, x)

void console_lv_set(unsigned char ch);
void console_lv_clear(unsigned char ch);
unsigned int console_lv_get(unsigned char ch);


void app_sleep(volatile long *ptcnt, long u_ms);
unsigned char app_pend_wake(long tcnt);

unsigned char app_capture_interval(long tcnt, long u_ms);
void app_update_interval(volatile long *ptcnt);



#endif

