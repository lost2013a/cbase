#ifndef _NSTAR_QUEUE_H_
#define _NSTAR_QUEUE_H_
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#define NSTAR_QUEUE_HEAD_INVALID 	0
#define NSTAR_QUEUE_HEAD_VALID 		1
#define NSTAR_HANDLE_INVALID 0


struct _queue_slide{
	unsigned int memsize;
	unsigned int usSize;
	unsigned int maxUsSize;
	unsigned int indexSend;
	unsigned int indexGet;

	long mutex;
	unsigned char *p_buf;
	unsigned short packMaxLen;
};


#if 0
#include <pthread.h>
#define nstar_adt_MutexCreate(p_mutex)	 pthread_mutex_init((pthread_mutex_t*)p_mutex, NULL)
#define _MUTEX_LOCK_QUEUE(p_mutex)		pthread_mutex_lock((pthread_mutex_t*)p_mutex)
#define _MUTEX_UNLOCK_QUEUE(p_mutex)	pthread_mutex_unlock((pthread_mutex_t*)p_mutex)
#else

#define nstar_adt_MutexCreate(...)	 
#define _MUTEX_LOCK_QUEUE(...)		
#define _MUTEX_UNLOCK_QUEUE(...)	

#endif

struct _queue_slide* nstar_queue_slide_init(unsigned int memsize,unsigned short packMaxLen);
unsigned char nstar_queue_slide_destroy(struct _queue_slide* handle);
unsigned char nstar_queue_slide_send(struct _queue_slide* handle,const unsigned char*p_buf,unsigned int len,unsigned char type);
unsigned char nstar_queue_slide_get(struct _queue_slide* handle,unsigned char*p_out,unsigned int *p_inOutLen);
unsigned char nstar_queue_slide_clear(struct _queue_slide* handle);



#endif 


