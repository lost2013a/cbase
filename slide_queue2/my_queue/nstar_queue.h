#ifndef _NSTAR_QUEUE_H_
#define _NSTAR_QUEUE_H_
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>


#define Q_MUTEX_HANDLE pthread_mutex_t
#define Q_MutexCreate(p_mutex)	 pthread_mutex_init(&p_mutex, NULL)
#define Q_MutexDelete(p_mutex)	pthread_mutex_destroy(&p_mutex)
#define Q_MutexLock(p_mutex)		pthread_mutex_lock(&p_mutex)
#define Q_MutexUnLock(p_mutex)	pthread_mutex_unlock(&p_mutex)




struct _queue_slide{
	Q_MUTEX_HANDLE mutex;
	unsigned int memsize;
	unsigned int usSize;
	unsigned int maxUsSize;
	unsigned int indexSend;
	unsigned int indexGet;
	unsigned char *p_buf;
	unsigned short packMaxLen;
};



struct _queue_slide* nstar_queue_slide_init(unsigned int memsize,unsigned short packMaxLen);
unsigned char nstar_queue_slide_destroy(struct _queue_slide* handle);
unsigned char nstar_queue_slide_send(struct _queue_slide* handle,const unsigned char*p_buf,unsigned int len);
unsigned char nstar_queue_slide_get(struct _queue_slide* handle,unsigned char*p_out,unsigned int *p_inOutLen);
unsigned char nstar_queue_slide_clear(struct _queue_slide* handle);



#endif 


