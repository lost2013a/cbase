#include "nstar_queue.h"

#define _SIZE_NSTAR_QUEUE_SLIDE  (sizeof(struct _queue_slide))

#define _QUEUE_POS_SEND(p_slideQueue,num)  (((p_slideQueue->indexSend+num))%(p_slideQueue->memsize))
#define _QUEUE_POS_GET(p_slideQueue,num)   (((p_slideQueue->indexGet+num))%(p_slideQueue->memsize))

#define _SIZE_QUEUE_PACK	(2) 

static void __slide_dta_cpy_len(struct _queue_slide *p_slideQueue,unsigned short len)
{
	unsigned char cnt=2;
	unsigned char *data= (unsigned char*)&len;
	while(cnt--)
	{
		p_slideQueue->p_buf[p_slideQueue->indexSend] = *(data++);
		p_slideQueue->indexSend = _QUEUE_POS_SEND(p_slideQueue,1);
		p_slideQueue->usSize++;
	}
	
}

static unsigned short __slide_dta_get_len(struct _queue_slide *p_slideQueue)
{
	unsigned short len;
	unsigned char cnt=2;
	unsigned char *data= (unsigned char*)&len;
	while(cnt--)
	{
		*(data++) = p_slideQueue->p_buf[p_slideQueue->indexGet];
		p_slideQueue->indexGet = _QUEUE_POS_GET(p_slideQueue,1);
		p_slideQueue->usSize--;
	}
	return len;
}

static void _slide_dta_cpy_send(struct _queue_slide *p_slideQueue,const unsigned char*p_buf,unsigned short len,unsigned char type)
{
	unsigned int offTmp;
	__slide_dta_cpy_len(p_slideQueue,len);
	if(p_slideQueue->indexSend+len <= p_slideQueue->memsize){
		memcpy(p_slideQueue->p_buf+p_slideQueue->indexSend,(void *)p_buf,len);
		p_slideQueue->indexSend = _QUEUE_POS_SEND(p_slideQueue,len);
	}else{
		offTmp = p_slideQueue->memsize - p_slideQueue->indexSend;
		memcpy(p_slideQueue->p_buf+p_slideQueue->indexSend,(void*)p_buf,offTmp);
		p_slideQueue->indexSend = len - offTmp;
		memcpy(p_slideQueue->p_buf,(char*)p_buf+offTmp,p_slideQueue->indexSend);
	}
	p_slideQueue->usSize += len;
}

static unsigned int _slide_dta_cpy_get(struct _queue_slide *p_slideQueue,unsigned char*p_buf)
{
	unsigned int getLen,offtmp;
	getLen = __slide_dta_get_len(p_slideQueue);	
	if(p_slideQueue->indexGet+getLen <= p_slideQueue->memsize){
		memcpy(p_buf,p_slideQueue->p_buf+p_slideQueue->indexGet,getLen);
		p_slideQueue->indexGet = _QUEUE_POS_GET(p_slideQueue,getLen);//(p_slideQueue->indexGet+getLen)%p_slideQueue->memsize;
	}else{
		offtmp = p_slideQueue->memsize - p_slideQueue->indexGet;
		memcpy(p_buf,p_slideQueue->p_buf+p_slideQueue->indexGet,offtmp);
		p_slideQueue->indexGet = getLen-offtmp;
		memcpy(p_buf+offtmp,p_slideQueue->p_buf,p_slideQueue->indexGet);
	}
	p_slideQueue->usSize -= getLen ;
	return getLen;
}




struct _queue_slide* nstar_queue_slide_init(unsigned int memsize,unsigned short packMaxLen)
{
	struct _queue_slide *p_slideQueue;
	p_slideQueue = (struct _queue_slide *)malloc(_SIZE_NSTAR_QUEUE_SLIDE);

	nstar_adt_MutexCreate(p_slideQueue->mutex);

	p_slideQueue->memsize = memsize;
	p_slideQueue->p_buf = malloc(memsize);
	p_slideQueue->packMaxLen = packMaxLen;
	if(p_slideQueue->p_buf == NULL){
		printf("queue malloc err\n");
	}
	return (struct _queue_slide*)p_slideQueue;
}

unsigned char nstar_queue_slide_destroy( struct _queue_slide* handle)
{
	struct _queue_slide *p_slideQueue;
	if(handle != NULL){
		p_slideQueue = (struct _queue_slide *)handle;
		//nstar_adt_MutexDelete(p_slideQueue->mutex);
		free(p_slideQueue->p_buf);
		free(p_slideQueue);
	}
	return 1;
}

unsigned char nstar_queue_slide_get(struct _queue_slide* handle,unsigned char*p_out,unsigned int *p_inOutLen)
{
	unsigned char ret = 0;
	struct _queue_slide *p_slideQueue;
	if(handle != NULL){
		p_slideQueue = (struct _queue_slide*)handle;
		_MUTEX_LOCK_QUEUE(p_slideQueue->mutex);
		if(p_slideQueue->usSize > _SIZE_QUEUE_PACK){
			if(*p_inOutLen >= p_slideQueue->packMaxLen){
				*p_inOutLen = _slide_dta_cpy_get(p_slideQueue,p_out);
				ret = 1;
			}		
		}
		_MUTEX_UNLOCK_QUEUE(p_slideQueue->mutex);
	}
	return ret;
}

unsigned char  nstar_queue_slide_send(struct _queue_slide* handle,const unsigned char*p_buf,unsigned int len,unsigned char type)
{
	unsigned char ret = 0;
	struct _queue_slide *p_slideQueue;
	unsigned int tmpLen = len;
	if(handle != NULL){
		p_slideQueue = (struct _queue_slide*)handle;	
		_MUTEX_LOCK_QUEUE(p_slideQueue->mutex);	

		if((tmpLen <= p_slideQueue->packMaxLen) && (p_slideQueue->usSize + tmpLen + _SIZE_QUEUE_PACK <= p_slideQueue->memsize)){
			_slide_dta_cpy_send(p_slideQueue,p_buf,len,type);
			if( p_slideQueue->maxUsSize < p_slideQueue->usSize ){
				p_slideQueue->maxUsSize = p_slideQueue->usSize;
			}		
			ret = 1;
		}
		else{
			if(((p_slideQueue->usSize + p_slideQueue->indexGet)%p_slideQueue->memsize) != p_slideQueue->indexSend){
				p_slideQueue->usSize = 0;
				p_slideQueue->indexGet = p_slideQueue->indexSend;
			}
		}
		_MUTEX_UNLOCK_QUEUE(p_slideQueue->mutex);
	}
	return ret;
}

unsigned char  nstar_queue_slide_clear(struct _queue_slide* handle)
{
	unsigned char ret = 0;
	struct _queue_slide *pBCQueue = (struct _queue_slide *)handle;
	if(pBCQueue != NULL){
		_MUTEX_LOCK_QUEUE(pBCQueue->mutex);
		pBCQueue->indexGet = pBCQueue->indexSend;
		pBCQueue->usSize = 0;
		_MUTEX_UNLOCK_QUEUE(pBCQueue->mutex);
		ret = 1;
	}
	return ret;
}

