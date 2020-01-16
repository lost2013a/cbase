#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#define MCAST_PORT 1212
#define MCAST_ADDR "239.12.12.12"
#define BUFLEN 1460
char buff[BUFLEN];



#define __PACKED__        __attribute__ ((__packed__)) 

struct _RTP_FIXED_HEADER
{
  
    unsigned char csrc_len:4;        /**//* expect 0 */
	unsigned char extension:1;        /**//* expect 1, see RTP_OP below */
	unsigned char padding:1;        /**//* expect 0 */
	unsigned char version:2;        /**//* expect 2 */
	
    /**//* byte 0 */
    /**//* bytes 2, 3 */
    /**//* byte 1 */
    
    unsigned char payload:7;        /**//* RTP_PAYLOAD_RTSP */
	unsigned char marker:1;        /**//* expect 1 */
	
    unsigned short seq_no;            
    /**//* bytes 4-7 */
    unsigned int timestamp;        
    /**//* bytes 8-11 */
    unsigned int ssrc;            /**//* stream number is used here. */
} __PACKED__;

typedef struct _RTP_FIXED_HEADER RTP_FIXED_HEADER;




static rec_ts(int len, unsigned char *buf)
{

	static unsigned short last_pid;
	static unsigned char s_tsCount ;
	unsigned int tsCount=0;
	unsigned short pid,audioPid,cal=0;
	int i=0;
	audioPid= 6501;
	unsigned char *p_dta= buf;

	for(i=0;i< 188;i++){
		cal+=p_dta[i];
	}	

	if(p_dta[0] == 0x47 ){
		pid = (p_dta[1] << 8 | p_dta[2])&0x1fff;
		
		if(pid != audioPid){
			p_dta+=188;
			len -= 188;
			return;
		}
		tsCount = p_dta[3]&0xf;

		printf("len %d[%02d]: ",len, tsCount);
		for(i=0;i< 20;i++){
			printf("%02x ", p_dta[i]);

		}	
		for(i=0;i< 188;i++){
			cal+=p_dta[i];
		}	
		printf("cal =%d \r\n", cal);	
			//s_tsCount = (tsCount+1)%0x10;	

			
	}
	else{
		return;
	}
	p_dta+=188;
	len -= 188;
	printf("\n");
}

#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#define DBG_FILE "test.mp3"
#define max_dbg_len (1024*20)
static unsigned char dbg_wbuf[max_dbg_len];

static FILE *file_p;
static void write_flush(void);

static int write_file(unsigned char *buf, unsigned int len)
{
	if(fwrite(buf, 1, len ,file_p) < 0){
		printf("erro, log write failed\n");	
		return -1;
	}
	write_flush();
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




int main(int argc, char*argv[])
{
	int s, rlen, err = -1;
	struct sockaddr_in local_addr;
	struct ip_mreq mreq;
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == -1)
	{
		perror("pg socket err");
		return -1;
	}
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(MCAST_PORT);
	err = bind(s,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
	if(err < 0)
	{
		perror("pg bind err");
		return -2;
	}
	mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	err = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
	if (err < 0)
	{
		perror("pg add multiaddr err");
		return -3;
	}

	log_file_open();
	while(1)	
	{	
		rlen = recv(s, buff, BUFLEN, 0);	
		if( rlen < 0){
			perror("pg recv err");
		}
		else if(rlen > 12){

		

			RTP_FIXED_HEADER *phead= (RTP_FIXED_HEADER *)buff;
			unsigned short p_seq=  ntohs(phead->seq_no);
			printf("seq:%x\n", p_seq);
			//rec_ts(rlen, buff);
			write_file(&buff[16], rlen-16);
			//memset(buff, 0, BUFLEN);
			printf("write %d\n", rlen);
		}
		usleep(5000);
	}
	setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
	close(s);
	return 0;
}





