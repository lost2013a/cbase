#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "nstar_queue.h"

#define MP3_BUFF_SIZE 		100
#define MP3_FRAME_MAX_LEN 	20

struct _queue_slide *mp3_queue;
#define QUEUE_GET(p_out,p_inOutLen) nstar_queue_slide_get(mp3_queue, p_out, p_inOutLen)
#define QUEUE_PUT(p_dta,len)   		nstar_queue_slide_send(mp3_queue, p_dta,len)


void mp3_queue_init(void)
{
	mp3_queue = nstar_queue_slide_init(MP3_BUFF_SIZE, MP3_FRAME_MAX_LEN);
}

void mp3_buf_write(unsigned char *data, unsigned int len)
{
	if(len <= MP3_FRAME_MAX_LEN){
		nstar_queue_slide_send(mp3_queue, data, len);
	}
	else{
		printf("mp3 data len too long\n");
	}
}

unsigned int mp3_buf_read(unsigned char *buf, unsigned int len)
{
	unsigned int idx=0, frame;
	while(idx+MP3_FRAME_MAX_LEN < len)
	{
		frame= MP3_FRAME_MAX_LEN;
		if(1 == nstar_queue_slide_get(mp3_queue, &buf[idx], &frame))
			idx+= frame;
		else
			break;
	}
	return idx;
}


#if 0
int main(int argc, char *argv[]) 
{	

	unsigned char buf[500]={0};
	mp3_queue_init();
	const char *str= "123456789abcdefghi123456";
	int i;

	mp3_buf_write((unsigned char*)str, 20);
	mp3_buf_write((unsigned char*)str, 20);
	mp3_buf_write((unsigned char*)str, 20);
	mp3_buf_write((unsigned char*)str, 20);
	mp3_buf_write((unsigned char*)str, 20);
	//printf("init ok \n");
	
	
	unsigned int len= mp3_buf_read(buf, 100);

	if(len > 0){
		buf[len]=0;
		printf("read %d: %s\n", len, buf);
	}
	return 0;
}
#endif



