#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "app_delay.h"



#define DBG_FILE "write_log.txt"
#define max_dbg_len (1024*20)
static unsigned char dbg_wbuf[max_dbg_len];
static int dbg_fd = -1;


int dbg_write_open(void)
{
	char sz_filename[20] ;
	sprintf(sz_filename, DBG_FILE);
	//dbg_fd = open(sz_filename, O_RDWR | O_CREAT |O_TRUNC); 
	dbg_fd = open(sz_filename, O_RDWR | O_CREAT |O_TRUNC, 0777 );//GCC BUG
	
	if(dbg_fd < 0){
		printf("open file \"%s\" failed, errno=%d.\n",
		sz_filename, errno);
		return -1;
	}
	printf("open ok :%d\n",dbg_fd);
	return 0;
}

void write_printf(char* fmt,...)  
{  
	int ret;
	unsigned int len;
	unsigned char* pbuf = dbg_wbuf;
	char buf[30];
	unsigned int time;
	va_list ap;
	va_start(ap,fmt);
	time= app_get_systime();
	len= snprintf(buf, 30, "WDBG[%04d.%02d]:\r\n",(time/1000)%1000, (time%1000));
	ret = write(dbg_fd, buf, len);
	if(ret <0 )
		printf("erro, write failed\n");	
	
	len=vsnprintf((char*)pbuf, max_dbg_len, fmt,ap);
	va_end(ap);
	ret = write(dbg_fd, dbg_wbuf, len);
	if(ret <0 )
		printf("erro, write failed\n");	
	ret=write(dbg_fd, "\n\n", 2);
	fsync(dbg_fd);
}

void write_dbg(unsigned char *p_dta, unsigned int dtalen , char* head)
{  	
	int ret,len;
	unsigned int time= app_get_systime();
	char buf[40];
	//if(head != NULL)
		len= snprintf(buf, 40, "WDBG:%d (%s)[%04d.%02d]:\r\n", dtalen, head, (time/1000)%1000, (time%1000));
	ret = write(dbg_fd, buf, len);
	if(ret <0 )
		printf("erro, write failed\n");	
	ret = write(dbg_fd, p_dta, dtalen);
	if(ret <0 )
		printf("erro, write failed\n");	
	ret=write(dbg_fd, "\n\n", 2);
	fsync(dbg_fd);

}




