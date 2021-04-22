#ifndef _BCONN_H__
#define _BCONN_H__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#define FD_INVALID 0
#define bc_dbg(fmt, args...)\
do {\
    printf("%s "fmt, b->name, ##args);\
}while(0)

#define DEFAULT_TR_OUT (10*1000)
#define BCONN_INIT_STRUCTURE(p_name, n_ip, n_port, p_rfunc, tr_out){ \
	p_name,\
	FD_INVALID,\
	n_ip,\
	n_port,\
	p_rfunc,\
	tr_out,\
	0,\
}
struct bsockt{
    char *name;
    int fd;
    unsigned int ipaddr; 
	unsigned short port;
    int (*pfunc_r)(unsigned char*, unsigned int);
    unsigned int rec_tout;  /*接收的阻塞时间，无特殊需求10*1000即可*/
    /*以下的，无需初始化值*/
    volatile unsigned char isNeedCreat;
	volatile long timer_step;
    volatile long timer_r;
    volatile long timer_s;
	unsigned int cnt_r;
    unsigned int cnt_s;
	unsigned int speed_r;
	unsigned int speed_s;
};
typedef struct bsockt BSOCKET;

#define SLEPP_T_R(n)  app_sleep(&b->timer_r, n*1000)
#define PEND_T_R()  app_pend_wake(b->timer_r)

#define SLEPP_T_S(n)  app_sleep(&b->timer_s, n*1000)
#define PEND_T_S()  app_pend_wake(b->timer_s)

void bconn_machine(struct bsockt *b);
void bconn_connect(struct bsockt *b, unsigned int remote_ip, unsigned short remote_port);
void bconn_stop(struct bsockt *b);
void bconn_restart(struct bsockt *b);
int bconn_send(struct bsockt *b, unsigned char *data, unsigned int len);
void bconn_block_rec(struct bsockt *b);

#define bconn_isConnect(b) \
({ \
	b->fd != FD_INVALID ? 1: 0; \
})

#define bconn_speed_r(b) \
({ \
	b->speed_r; \
})

#define bconn_speed_s(b) \
({ \
	b->speed_s; \
})


#endif

