#ifndef   _CONSOLE_H_
#define   _CONSOLE_H_

#define pr_fmt(fmt) ""fmt
#define console_cond(cond, fmt, args...)\
do {\
    if(cond)\
        printf(pr_fmt(fmt), ##args);\
}while(0)
#define console_log(ch, fmt, args...) console_cond(console_lv_get(ch), fmt, ##args) 
#define console_0_dbg(fmt...) console_log(0,##fmt) 
#define console_1_dbg(fmt...) console_log(1,##fmt) 
#define console_2_dbg(fmt...) console_log(2,##fmt) 
#define console_3_dbg(fmt...) console_log(3,##fmt) 

#define myerro  	printf
#define mydbg		console_0_dbg
#define main_dbg	console_1_dbg
#define iic_dbg		console_2_dbg



void console_lv_set(unsigned char ch);
void console_lv_clear(unsigned char ch);
unsigned int console_lv_get(unsigned char ch);
void argdbg(unsigned int arg, char *str);
void argdbg(unsigned int arg, char *str);
#define myarg(arg) argdbg(arg,#arg)
#define myargx(arg) argdbgx(arg,#arg)

#endif


