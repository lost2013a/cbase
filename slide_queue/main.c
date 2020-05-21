#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "nstar_queue.h"

struct _queue_slide *mp3_queue;
#define QUEUE_GET(p_out,p_inOutLen) nstar_queue_slide_get(mp3_queue, p_out, p_inOutLen)
#define QUEUE_PUT(p_dta,len)   		nstar_queue_slide_send(mp3_queue, p_dta,len, 0)


#if 1
#define QUEUE_MEM_SIZE 100
#define QUEUE_PACK_MAX 20
#else

#define QUEUE_MEM_SIZE (1024*1024*2)
#define QUEUE_PACK_MAX 1024
#endif

void mp3_queue_init(void)
{
	mp3_queue = nstar_queue_slide_init(QUEUE_MEM_SIZE, QUEUE_PACK_MAX);
}

void mp3_data_input(unsigned char *data, unsigned int len)
{
	if(len <= QUEUE_PACK_MAX){
		nstar_queue_slide_send(mp3_queue, data, len, 0);
	}
	else{
		printf("mp3 data len too long\n");
	}
}






int main(int argc, char *argv[]) 
{	

	unsigned char buf[500]={0};
	mp3_queue = nstar_queue_slide_init(QUEUE_MEM_SIZE, QUEUE_PACK_MAX);
	const char *str= "123456789abcdefghi123456";
	int i;

	QUEUE_PUT(str, 8);
	QUEUE_PUT(str, 12);
	//printf("init ok \n");
	
	unsigned int len, ret;
	for(i=0; i< 5; i++){
		len=QUEUE_PACK_MAX;
		ret= QUEUE_GET(buf, &len);
		if(ret == 1){
			buf[len]= 0;
			printf("len=%d, buf=%s\n",len, buf);
		}
	}
	return 0;
}




