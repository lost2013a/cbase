#ifndef	_EBM_NET_H
#define _EBM_NET_H


struct bsockt{
	int fd;
	unsigned int lastTimeSec;
	unsigned int sndDtaCnt;
	unsigned int speedCnt;
	unsigned int speed;
	volatile unsigned int ipaddr;
	volatile unsigned short port;
	unsigned char keepSec;
	volatile unsigned char isNeedCreat;
};
#define FD_INVALID 0

unsigned int app_get_systime(void);


void command_net_start(unsigned int remote_ip, unsigned short remote_port);
void command_net_stop(void);
void command_net_restart(void);
void command_net_loop(void);

void rtp_net_start(unsigned int remote_ip, unsigned short remote_port);
void rtp_net_stop(void);
void rtp_net_restart(void);
void rtp_net_loop(void);



#endif


