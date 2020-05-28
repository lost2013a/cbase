#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "nstar_queue.h"

struct _queue_slide *mp3_queue;
#define QUEUE_GET(p_out,p_inOutLen) nstar_queue_slide_get(mp3_queue, p_out, p_inOutLen)
#define QUEUE_PUT(p_dta,len)   		nstar_queue_slide_send(mp3_queue, p_dta,len)

void mp3_queue_init(void)
{
#define QUEUE_MEM_SIZE 100
#define QUEUE_PACK_MAX 20
	mp3_queue = nstar_queue_slide_init(QUEUE_MEM_SIZE, QUEUE_PACK_MAX);
}

void mp3_buf_write(unsigned char *data, unsigned int len)
{
	if(len <= QUEUE_PACK_MAX){
		nstar_queue_slide_send(mp3_queue, data, len);
	}
	else{
		printf("mp3 data len too long\n");
	}
}

//int mp3_buf_read(unsigned char *data, unsigned int len)
int mp3_buf_read(void)
{
	unsigned char buf[500]={0};
	unsigned int len;
	len=QUEUE_PACK_MAX;
	unsigned char ret= nstar_queue_slide_get(mp3_queue, buf, &len);
	if(ret == 1){
		buf[len]= 0;
		printf("len=%d, buf=%s\n",len, buf);
	}
}





int main(int argc, char *argv[]) 
{	

	unsigned char buf[500]={0};
	mp3_queue_init();
	const char *str= "123456789abcdefghi123456";
	int i;

	mp3_buf_write((unsigned char*)str, 8);
	mp3_buf_write((unsigned char*)str, 12);
	//printf("init ok \n");
	
	for(i=0; i< 5; i++){
		mp3_buf_read();
	}
	return 0;
}




