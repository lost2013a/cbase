#include<stdio.h> 
#include<string.h>   
#include<stdlib.h>   
#include "rec.h"


typedef struct _SYS_TIME
{
	volatile unsigned short year;
	volatile unsigned char  mon;
	volatile unsigned char  day;
	volatile unsigned char  hour;
	volatile unsigned char  min;
	volatile unsigned char  sec;
	volatile unsigned short msec;
	volatile unsigned long  usec;
	volatile unsigned long  total_sec;
}SYS_TIME;	


struct dev_Log
{
	struct dev_Log *nexp;
	SYS_TIME warn_time;	//告警时间
	unsigned int mem_id;	//事件号
	unsigned int index;		//序号
	unsigned int value;		//值
};

static unsigned char param_w_flag= 0;
static unsigned char param_t_flag= 0;


#define log_dbg printf




int main(int argc, char *argv[]) 
{	



}




