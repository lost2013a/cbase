#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "nstar_queue.h"

struct _queue_slide *g_queueHandle;
#define QUEUE_GET(p_out,p_inOutLen) nstar_queue_slide_get(g_queueHandle,p_out,p_inOutLen)
#define QUEUE_PUT(p_dta,len)   		nstar_queue_slide_send(g_queueHandle,p_dta,len,'h')

int main(int argc, char *argv[]) 
{	

#define QUEUE_MEM_SIZE 500
#define QUEUE_PACK_MAX 10
	unsigned char buf[10]={0};
	g_queueHandle = nstar_queue_slide_init((nstar_u8*)"front panel",QUEUE_MEM_SIZE,QUEUE_PACK_MAX,1);
	const char *str= "12345678";
	int i;
//	for(i=0; i< 2; i++){
//		QUEUE_PUT(str, 3);
//	}
	nstar_queue_slide_send(g_queueHandle,str,3,'a');
	nstar_queue_slide_send(g_queueHandle,str,4,'b');
	nstar_queue_slide_send(g_queueHandle,str,5,'c');
	dbg_q();
	unsigned int len, rlen;
	for(i=0; i< 3; i++){
		len=QUEUE_PACK_MAX;
		rlen= QUEUE_GET(buf, &len);
		buf[9]= 0;
		printf("len=%d, rlen=%d, buf=%s\n",len, rlen, &buf[0]);
	}
	dbg_q();
	return 0;
}




