#include "nstar_queue.h"

#if 1
#define dbg printf
static void argdbg(unsigned int arg, char *str)
{
	dbg("%s=%d\r\n", str, arg);
}
static void argdbgx(unsigned int arg, char *str)
{
	dbg("%s=%x\r\n", str, arg);
}

#define myarg(arg) argdbg(arg,#arg)
#define myargx(arg) argdbgx(arg,#arg)



#define _SIZE_NSTAR_QUEUE_SLIDE  (sizeof(struct _queue_slide))


#define _QUEUE_POS_SEND(p_slideQueue,num)  (((p_slideQueue->indexSend+num))%(p_slideQueue->memsize))
#define _QUEUE_POS_GET(p_slideQueue,num)   (((p_slideQueue->indexGet+num))%(p_slideQueue->memsize))

#define _SIZE_QUEUE_PACK	(2) //sizeof(nstar_u16)

static struct _queue_slide *h_slideQueue;
void dbg_q(void)
{
	int i;
	struct _queue_slide *pq= h_slideQueue;
	printf("h_slideQueue:\n");
	printf("usSize:\t%d\n", pq->usSize);
	printf("iSend:\t%d\n", pq->indexSend);
	printf("iGet:\t%d", pq->indexGet);
	
	for(i=0;i<pq->usSize;i++ ){
		if(i%16 == 0)
			printf("\n");
		printf("%02X ", pq->p_buf[i]);	
	}
	printf("\n");	
}
#endif

static void __slide_dta_cpy_byte(struct _queue_slide *p_slideQueue,nstar_u8 dta)
{
	p_slideQueue->p_buf[p_slideQueue->indexSend] = dta;
	p_slideQueue->indexSend = _QUEUE_POS_SEND(p_slideQueue,1);
	p_slideQueue->usSize++;
}

static void __slide_dta_cpy_len(struct _queue_slide *p_slideQueue,nstar_u16 len)
{
	if(p_slideQueue->isHadHead == NSTAR_QUEUE_HEAD_VALID ){
		len++;
	}
	__slide_dta_cpy_byte(p_slideQueue,len>>8);
	__slide_dta_cpy_byte(p_slideQueue,len&0xff);
}

static nstar_u8 __slide_dta_get_byte(struct _queue_slide *p_slideQueue)
{
	nstar_u8 dta;
	dta = p_slideQueue->p_buf[p_slideQueue->indexGet];
	p_slideQueue->indexGet = _QUEUE_POS_GET(p_slideQueue,1);
	p_slideQueue->usSize--;
	return dta;
}
static nstar_u16 __slide_dta_get_len(struct _queue_slide *p_slideQueue)
{
	nstar_u16 len;
	len = __slide_dta_get_byte(p_slideQueue) << 8;
	len |= __slide_dta_get_byte(p_slideQueue);
	return len;
}


static void _slide_dta_cpy_send(struct _queue_slide *p_slideQueue,const nstar_u8*p_buf,nstar_u16 len,nstar_u8 type)
{
	nstar_u32 offTmp;
	__slide_dta_cpy_len(p_slideQueue,len);
	if(p_slideQueue->isHadHead == NSTAR_QUEUE_HEAD_VALID){
		__slide_dta_cpy_byte(p_slideQueue,type);
	}

	if(p_slideQueue->indexSend+len <= p_slideQueue->memsize){
		nstar_adt_memcpy(p_slideQueue->p_buf+p_slideQueue->indexSend,(void *)p_buf,len);
		p_slideQueue->indexSend = _QUEUE_POS_SEND(p_slideQueue,len);
	}else{
		offTmp = p_slideQueue->memsize - p_slideQueue->indexSend;
		nstar_adt_memcpy(p_slideQueue->p_buf+p_slideQueue->indexSend,(void*)p_buf,offTmp);
		p_slideQueue->indexSend = len - offTmp;
		nstar_adt_memcpy(p_slideQueue->p_buf,(char*)p_buf+offTmp,p_slideQueue->indexSend);
	}
	p_slideQueue->usSize += len;
}
static nstar_u32 _slide_dta_cpy_get(struct _queue_slide *p_slideQueue,nstar_u8*p_buf)
{
	nstar_u32 getLen,offtmp;
	getLen = __slide_dta_get_len(p_slideQueue);	
	if(p_slideQueue->indexGet+getLen <= p_slideQueue->memsize){
		nstar_adt_memcpy(p_buf,p_slideQueue->p_buf+p_slideQueue->indexGet,getLen);
		p_slideQueue->indexGet = _QUEUE_POS_GET(p_slideQueue,getLen);//(p_slideQueue->indexGet+getLen)%p_slideQueue->memsize;
	}else{
		offtmp = p_slideQueue->memsize - p_slideQueue->indexGet;
		nstar_adt_memcpy(p_buf,p_slideQueue->p_buf+p_slideQueue->indexGet,offtmp);
		p_slideQueue->indexGet = getLen-offtmp;
		nstar_adt_memcpy(p_buf+offtmp,p_slideQueue->p_buf,p_slideQueue->indexGet);
	}
	p_slideQueue->usSize -= getLen ;
	return getLen;
}



nstar_handle nstar_queue_slide_init(nstar_u8*p_name,nstar_u32 memsize,nstar_u16 packMaxLen,nstar_u8 isHadHead)
{
	struct _queue_slide *p_slideQueue;
	p_slideQueue = (struct _queue_slide *)nstar_adt_malloc(_SIZE_NSTAR_QUEUE_SLIDE);
#if 1
	h_slideQueue = p_slideQueue;
#endif
	
	nstar_adt_MutexCreate(&p_slideQueue->mutex);

	p_slideQueue->memsize = memsize;//200*1024
	p_slideQueue->p_buf = nstar_adt_malloc(memsize);
	p_slideQueue->p_name = p_name;
	p_slideQueue->packMaxLen = packMaxLen;
	p_slideQueue->isHadHead = isHadHead;
	if(p_slideQueue->p_buf == NSTAR_NULL){
		nstar_adt_printk("queue ERR %s malloc NULL \n",p_name);
	}
	return (nstar_handle)p_slideQueue;
}
NSTAR_BOOL nstar_queue_slide_destroy( nstar_handle handle)
{
	struct _queue_slide *p_slideQueue;
	if(handle != NSTAR_HANDLE_INVALID){
		p_slideQueue = (struct _queue_slide *)handle;
		nstar_adt_MutexDelete(p_slideQueue->mutex);
		nstar_adt_free(p_slideQueue->p_buf);
		nstar_adt_printk(" queue_slide_destroy	%s ok\n",p_slideQueue->p_name);
		nstar_adt_free(p_slideQueue);
	}
	return NSTAR_TRUE;
}


NSTAR_BOOL nstar_queue_slide_get(nstar_handle handle,nstar_u8*p_out,nstar_u32 *p_inOutLen)
{
	NSTAR_BOOL ret = NSTAR_FALSE;
	struct _queue_slide *p_slideQueue;
	if(handle != NSTAR_HANDLE_INVALID){
		p_slideQueue = (struct _queue_slide*)handle;
		_MUTEX_LOCK_QUEUE(p_slideQueue->mutex);
		if(p_slideQueue->usSize > _SIZE_QUEUE_PACK){
			if(*p_inOutLen >= p_slideQueue->packMaxLen){
				*p_inOutLen = _slide_dta_cpy_get(p_slideQueue,p_out);
				ret = NSTAR_TRUE;
			}		
		}
		_MUTEX_UNLOCK_QUEUE(p_slideQueue->mutex);
	}
	return ret;
}

static void _queue_debug_print(struct _queue_slide* pBCQueue)
{
//	struct nstar_time time;
//	nstar_adt_GetTime(&time);
//	if(pBCQueue->lastTimeSec + 15 < time.seconds)
//	{
//		nstar_adt_printk("%s maxUs us mem send get sendFailCnt[%d %d %d %d %d %d]\n",
//				pBCQueue->p_name,
//				pBCQueue->maxUsSize,
//				pBCQueue->usSize,
//				pBCQueue->memsize,
//				pBCQueue->indexSend,
//				pBCQueue->indexGet,
//				pBCQueue->sendFailCnt);
//		pBCQueue->lastTimeSec =  time.seconds;
//	}
}

NSTAR_BOOL  nstar_queue_slide_send(nstar_handle handle,const nstar_u8*p_buf,nstar_u32 len,nstar_u8 type)
{
	NSTAR_BOOL ret = NSTAR_FALSE;
	struct _queue_slide *p_slideQueue;
	nstar_u32 tmpLen = len;
	if(len > 0 && handle != NSTAR_HANDLE_INVALID){
		p_slideQueue = (struct _queue_slide*)handle;	
		if(p_slideQueue->isHadHead == NSTAR_QUEUE_HEAD_VALID){
			tmpLen++ ;
		}
		_MUTEX_LOCK_QUEUE(p_slideQueue->mutex);	

		if((tmpLen <= p_slideQueue->packMaxLen) && (p_slideQueue->usSize + tmpLen + _SIZE_QUEUE_PACK <= p_slideQueue->memsize)){
			_slide_dta_cpy_send(p_slideQueue,p_buf,len,type);
			if( p_slideQueue->maxUsSize < p_slideQueue->usSize ){
				p_slideQueue->maxUsSize = p_slideQueue->usSize;
			}		
			ret = NSTAR_TRUE;
		}
		else{
			p_slideQueue->sendFailCnt++;
			if(((p_slideQueue->usSize + p_slideQueue->indexGet)%p_slideQueue->memsize) != p_slideQueue->indexSend){
				p_slideQueue->usSize = 0;
				p_slideQueue->indexGet = p_slideQueue->indexSend;
			}
		}
		_queue_debug_print(p_slideQueue);
		_MUTEX_UNLOCK_QUEUE(p_slideQueue->mutex);
	}
	return ret;
}

NSTAR_BOOL  nstar_queue_slide_clear(nstar_handle handle)
{
	NSTAR_BOOL ret = NSTAR_FALSE;
	struct _queue_slide *pBCQueue = (struct _queue_slide *)handle;
	if(pBCQueue != NSTAR_NULL){
		_MUTEX_LOCK_QUEUE(pBCQueue->mutex);
		nstar_adt_printk("queue %s clear maxUs us mem send get sendFailCnt[%d %d %d %d %d %d]\n",
			pBCQueue->p_name,
			pBCQueue->maxUsSize,
			pBCQueue->usSize,
			pBCQueue->memsize,
			pBCQueue->indexSend,
			pBCQueue->indexGet,
			pBCQueue->sendFailCnt);
		pBCQueue->indexGet = pBCQueue->indexSend;
		pBCQueue->sendFailCnt = 0;
		pBCQueue->usSize = 0;
		_MUTEX_UNLOCK_QUEUE(pBCQueue->mutex);
		ret = NSTAR_TRUE;
	}
	return ret;
}

