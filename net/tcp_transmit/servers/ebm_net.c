#include "ebm_env.h"

#define TEST_IP (192 << 24 | 168 <<16 | 251 <<8 | 181)
#define TEST_PORT 50003

typedef int nstar_handle;


volatile unsigned short port;
volatile unsigned int ipaddr;

struct bsockt{
	nstar_handle fd;
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

static struct bsockt command_net_handle = {FD_INVALID,0,0};

unsigned int app_get_systime(void)
{	
#include <sys/time.h>

	struct timeval tv;
	gettimeofday(&tv, NULL);
#if 0
	return (unsigned int)(tv.tv_sec*1000 + tv.tv_usec/1000);
#else
	return (unsigned int)(tv.tv_sec);
#endif
}


static void _socket_close(int *fd)
{
	if(*fd != FD_INVALID){
		close(*fd);
		*fd = FD_INVALID;
	}
}


static int _socket_open(unsigned int ip, unsigned short port)
{
    int conn_sock;
    struct sockaddr_in	server_addr;
    conn_sock	= socket(AF_INET, SOCK_STREAM , 0);
    if (conn_sock < 0) {
        perror("socket(2) error\n");
        return  FD_INVALID;
    }

    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(port);
   	server_addr.sin_addr.s_addr = htonl(ip);
	
	if (connect(conn_sock,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)) >= 0) {
        printf("connect ok\n");
  
	}
	else{
		printf("wait connect to transmit addr failed %d.%d.%d.%d:%d\n",
				ip>>24, (ip&0xff0000)>>16, (ip&0xff00)>>8, ip&0xff, port);
		conn_sock= FD_INVALID;
	}
	return conn_sock;
}


static unsigned char _creat_connect_socket(struct bsockt *h_net)
{
	int fd=0;
	unsigned int ip;
	unsigned short port;
	unsigned char ret = 0;
	_socket_close(&h_net->fd);

	ip = h_net->ipaddr;
	port = h_net->port;	
	
	if(ip > 0 && port > 0 ){
		fd = _socket_open(ip, port);
		if(fd  != FD_INVALID ){
			h_net->fd = fd;
			h_net->lastTimeSec = 0;
			h_net->keepSec = 0;
			ret = 1;	
		}
	}	
	return ret;
}


static void command_net_machine(void *parm)
{
	struct bsockt *h_net= (struct bsockt *)parm;
	if(h_net->isNeedCreat == 3){
		h_net->isNeedCreat = 0;
		_socket_close(&h_net->fd);
	}
	else if(h_net->isNeedCreat == 1){
		if(1 == _creat_connect_socket(h_net)){
			h_net->isNeedCreat = 0;
		}
		else{
			h_net->isNeedCreat = 2;
			h_net->lastTimeSec = app_get_systime();
		}
	}
	else if(h_net->isNeedCreat == 2){
		if(h_net->lastTimeSec + 10 < app_get_systime()){
			if(1 == _creat_connect_socket(h_net)){
				h_net->isNeedCreat = 0;
			}
			else{
				h_net->lastTimeSec = app_get_systime();
			}
		}

	}
}

void command_net_start(unsigned int remote_ip, unsigned short remote_port)
{
	struct bsockt *h_net= &command_net_handle;
	h_net->isNeedCreat = 1;
	h_net->ipaddr= remote_ip;
	h_net->port= remote_port;
}
void command_net_stop(void)
{
	struct bsockt *h_net= &command_net_handle;
	h_net->isNeedCreat = 3;
}

void command_net_restart(void)
{
	struct bsockt *h_net= &command_net_handle;
	_socket_close(&h_net->fd);
	h_net->isNeedCreat = 2;
	h_net->lastTimeSec = app_get_systime() + 5;/*10 -5 =5s gaps*/
}


static void block_rec(unsigned int fd)
{
	static unsigned char rbuf[1460];
	int	nsel;
	struct	timeval timeover={0};
	fd_set rfd_set; 
	timeover.tv_sec = 1;
	timeover.tv_usec = 500000;
	FD_ZERO( &rfd_set );
	FD_SET(fd, &rfd_set );
	nsel = select( fd+1, &rfd_set, NULL, NULL, &timeover );
	if ( nsel > 0 )
	{
		if(FD_ISSET( fd, &rfd_set )){
			int rlen = read(fd, rbuf, 1460);
			if(rlen <= 0){
				printf("socket unvaild\n");
				command_net_restart();
			}
			else{
				//printf("rlen %d bytes\n", rlen);
				handle_ebm_msg(rbuf, rlen);
			}
				
		}
	}			
}


static void data_process(void)
{
	block_rec(command_net_handle.fd);
}


void command_net_loop(void)
{
	command_net_machine(&command_net_handle);
	if(command_net_handle.fd != FD_INVALID){
		data_process();
	}
	usleep(10*1000);
}







