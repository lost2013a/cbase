#ifndef	_EBM_SYS_H
#define _EBM_SYS_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h> 


#define _PRINT(format,...)\
do{\
	printf("[EBM] ");\
	printf(""format"", ##__VA_ARGS__);\
}while(0) 

#define ebm_dbg  _PRINT




#endif
