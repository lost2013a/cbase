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
	SYS_TIME warn_time;	//�澯ʱ��
	unsigned int mem_id;	//�¼���
	unsigned int index;		//���
	unsigned int value;		//ֵ
};

static unsigned char param_w_flag= 0;
static unsigned char param_t_flag= 0;


#define log_dbg printf




int main(int argc, char *argv[]) 
{	



}




