#ifndef _TS_MONITOR_H_
#define _TS_MONITOR_H_


struct rds_buf_t
{
	 int listenSn;
	 int clientSn;
	 unsigned short listenPort;
	 struct sockaddr_in to;	
	 unsigned int timer;
	 unsigned char *head;
	 unsigned char *tail;
	 unsigned char *ptr;
	 int size;
	 unsigned char send;
	 unsigned short pid;
	 unsigned char ts_cc;
	 unsigned int bitRate;
	 unsigned int packCntPerSec;
};




void parse_ts_msg(unsigned char *data, unsigned char type, unsigned short cnt, unsigned short len);

const char* webmsg_nstar_msg_onoff(unsigned char* data, unsigned short len, unsigned char cnt);


#endif
