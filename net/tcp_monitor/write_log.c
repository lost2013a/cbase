#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>


#define DBG_FILE "write_log.txt"
#define max_dbg_len (1024*20)
static unsigned char dbg_wbuf[max_dbg_len];

static FILE *file_p;

static int write_file(unsigned char *buf, unsigned int len)
{
	if(fwrite(buf, 1, len ,file_p) < 0){
		printf("erro, log write failed\n");	
		return -1;
	}
	if(fwrite("\n\n", 1, 2 ,file_p) < 0){
		printf("erro, log write failed\n");	
		return -1;
	}
	return 0;
}

static void write_flush(void)
{
	fflush(file_p);
	fsync(fileno(file_p));
}


int log_file_open(void)
{
	int ret=0;
	file_p = fopen(DBG_FILE, "w+");	
	if(file_p == NULL){
		printf("log file open failed\n");
		ret= -1;
	}
	printf("log_file_open\n");
	return ret;
}

void log_file_close(void)
{
	fclose(file_p);
}


void log_printf(char* fmt,...)  
{  
	unsigned int len;
	unsigned char* pbuf = dbg_wbuf;
	va_list ap;
	va_start(ap,fmt);
	len=vsnprintf((char*)pbuf, max_dbg_len, fmt,ap);
	va_end(ap);
	write_file(dbg_wbuf, len);
	write_flush();
}

void log_write(unsigned char *p_dta, unsigned int dtalen , char* head)
{  	
	if(head){
		if(fwrite((unsigned char *)head, 1, strlen(head) ,file_p) < 0){
			printf("erro, log write failed\n");	
			return;
		}
	}	
	write_file(p_dta, dtalen);
	write_flush();
}



//int main(int argc, char *argv[]) 
//{
//	const char*str= "world\n";
//	log_file_open();
//	log_printf("hello ");	
//	log_write((unsigned char*)str, strlen(str), "[LOG]:");	
//	log_printf("%s %s\n", __DATE__, __TIME__);	
//}


