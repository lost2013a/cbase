#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "nstar_queue.h"

struct _queue_slide *g_queueHandle;
#define QUEUE_GET(p_out,p_inOutLen) nstar_queue_slide_get(g_queueHandle,p_out,p_inOutLen)
#define QUEUE_PUT(p_dta,len)   		nstar_queue_slide_send(g_queueHandle,p_dta,len,'s')


#define MP3_TYPE 0x30

int main(int argc, char *argv[]) 
{	

#define QUEUE_MEM_SIZE 30
#define QUEUE_PACK_MAX 100
	unsigned char buf[500]={0};
	g_queueHandle = nstar_queue_slide_init((nstar_u8*)"front panel",QUEUE_MEM_SIZE,QUEUE_PACK_MAX,1);
	const char *str= "123456789abcdef";
	int i;

	nstar_queue_slide_send(g_queueHandle,str,8 , MP3_TYPE);
	nstar_queue_slide_send(g_queueHandle,str,10, MP3_TYPE);
	nstar_queue_slide_send(g_queueHandle,str,12, MP3_TYPE);

	unsigned int len, ret;
	for(i=0; i< 5; i++){
		len=QUEUE_PACK_MAX;
		ret= QUEUE_GET(buf, &len);
		if(ret == 1 && len > 1){
			len-=1;
			buf[len]= 0;
			printf("len=%d, buf=%s\n",len, &buf[1]);
		}
	}
	return 0;
}




