#ifndef _NSTAR_QUEUE_H_
#define _NSTAR_QUEUE_H_
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>


#define NSTAR_QUEUE_HEAD_INVALID 	0
#define NSTAR_QUEUE_HEAD_VALID 		1
#define NSTAR_HANDLE_INVALID (0)


#ifndef NSTAR_TRUE
#define NSTAR_TRUE 1
#endif
#ifndef NSTAR_FALSE
#define NSTAR_FALSE 0
#endif
#ifndef NSTAR_BOOL
#define NSTAR_BOOL nstar_u8
#endif
#ifndef NSTAR_NULL
#define NSTAR_NULL  0
#endif

//typedef nstar_u8 	unsigned char
//typedef nstar_u16 	unsigned short
//typedef nstar_u32 	unsigned int



typedef unsigned char nstar_u8;
typedef unsigned short nstar_u16;	
typedef unsigned int nstar_u32;	
//typedef int nstar_handle;


struct _queue_slide{
	nstar_u32 memsize;
	nstar_u32 usSize;
	nstar_u32 maxUsSize;
	nstar_u32 indexSend;
	nstar_u32 indexGet;

	int mutex;
	nstar_u8 *p_buf;
	nstar_u32 lastTimeSec; 
	nstar_u8* p_name;
	nstar_u8 isHadHead;
	nstar_u16 packMaxLen;
	nstar_u32 sendFailCnt;
};


typedef struct _queue_slide* nstar_handle;


#define nstar_adt_malloc malloc
#define nstar_adt_memcpy memcpy
#define nstar_adt_printk printf
#define nstar_adt_free free

#define nstar_adt_MutexDelete(...)	
#define nstar_adt_MutexCreate(...)	
#define _MUTEX_LOCK_QUEUE(...)	
#define _MUTEX_UNLOCK_QUEUE(...)	



//
nstar_handle nstar_queue_slide_init(nstar_u8*p_name,
										nstar_u32 memsize,
										nstar_u16 packMaxLen,
										nstar_u8 isHadHead);
NSTAR_BOOL nstar_queue_slide_destroy(nstar_handle handle);

NSTAR_BOOL nstar_queue_slide_send(nstar_handle handle,const nstar_u8*p_buf,nstar_u32 len,nstar_u8 type);
NSTAR_BOOL nstar_queue_slide_get(nstar_handle handle,nstar_u8*p_out,nstar_u32 *p_inOutLen);
NSTAR_BOOL nstar_queue_slide_clear(nstar_handle handle);

void dbg_q(void);

#endif //_NSTAR_QUEUE_H_


