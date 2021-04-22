#include "bconn.h"
#include "app_delay.h"


enum {
    NETSTEP_IDLE,
    NETSTEP_CREAT,
    NETSTEP_RECREAT,
    NETSTEP_STOP,
};
#define _STEP(n)    do{b->isNeedCreat = n;}while(0)  

#define SLEPP_T_STEP(n)  app_sleep(&b->timer_step, n*1000)
#define PEND_T_STEP()  app_pend_wake(b->timer_step)

static void _socket_close(int *fd)
{
	if(*fd != FD_INVALID){
		close(*fd);
		*fd = FD_INVALID;
	}
}

static int _socket_open(struct bsockt *b)
{
    int conn_sock;
    struct sockaddr_in	server_addr;
    conn_sock	= socket(AF_INET, SOCK_STREAM , 0);
    if (conn_sock < 0) {
	    bs_dbg("socket creat error\n");
	    return  FD_INVALID;
    }
    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
   	server_addr.sin_addr.s_addr = htonl(b->ipaddr);
    server_addr.sin_port		= htons(b->port);
	if (connect(conn_sock,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)) >= 0) {
        bs_dbg("command connect ok\n");
	}
	else{
        unsigned int ip = b->ipaddr;
		bs_dbg("wait connect to command addr failed %d.%d.%d.%d:%d\n",
				ip>>24, (ip&0xff0000)>>16, (ip&0xff00)>>8, ip&0xff, b->port);
		close(conn_sock);
		conn_sock= FD_INVALID;
	}	
	return conn_sock;
}

static unsigned char _creat_connect_socket(struct bsockt *b)
{
	unsigned char ret = 0;
	_socket_close(&b->fd);
	if(b->ipaddr > 0 && b->port > 0){
		int fd = _socket_open(b);
		if(fd != FD_INVALID){
			b->fd = fd;
			ret = 1;	
		}
	}	
	return ret;
}

void bconn_machine(struct bsockt *b)
{
    if(PEND_T_STEP()){
        switch(b->isNeedCreat) {
        case NETSTEP_CREAT:
        case NETSTEP_RECREAT:    
            if(_creat_connect_socket(b)) {
                _STEP(NETSTEP_IDLE);
            }else {
                _STEP(NETSTEP_RECREAT);
            }
            if(b->isNeedCreat == NETSTEP_RECREAT) {
                /*多等待几秒*/
                SLEPP_T_STEP(5);
            }
            break;     
        case NETSTEP_STOP:
            _STEP(NETSTEP_IDLE);
            _socket_close(&b->fd);
            break;
        default:
            break;    
        }
        SLEPP_T_STEP(2);
    }
}

void bconn_start(struct bsockt *b, unsigned int remote_ip, unsigned short remote_port)
{
    if(remote_ip)
	    b->ipaddr= remote_ip;
    if(remote_port)
	    b->port= remote_port;
    _STEP(NETSTEP_CREAT);
}
void bconn_stop(struct bsockt *b)
{
	_STEP(NETSTEP_STOP);
}

void bconn_restart(struct bsockt *b)
{
	if(b->fd != FD_INVALID){
		_socket_close(&b->fd);
        _STEP(NETSTEP_RECREAT);
	}
}

int bconn_send(struct bsockt *b, unsigned char *data, unsigned int len)
{
	int ret= write(b->fd, data, len);
	if(ret <= 0){
		printf("command send err=%d\n", ret);
		bconn_restart(b);
	}
	return ret;
}

unsigned char bconn_sta(struct bsockt *b)
{
	return b->fd != FD_INVALID ? 1: 0;
}


void bconn_block_rec(struct bsockt *b)
{
	static unsigned char rbuf[1460];
	int	nsel, fd = b->fd;
	struct	timeval timeover={0};
	fd_set rfd_set; 
	timeover.tv_sec = 0;
	timeover.tv_usec = 10*1000;
	FD_ZERO( &rfd_set );
	FD_SET(fd, &rfd_set );
	nsel = select( fd+1, &rfd_set, NULL, NULL, &timeover );
	if ( nsel > 0 )
	{
		if(FD_ISSET( fd, &rfd_set )){
			int rlen = read(fd, rbuf, 1460);
            printf("read %d\n", rlen);
			if(rlen <= 0){
				printf("socket unvaild\n");
				bconn_restart(b);
			}
			else{
                if(b->pfunc_r != NULL) {
				    b->pfunc_r(rbuf, rlen);
                }
			}	
		}
	}			
}






