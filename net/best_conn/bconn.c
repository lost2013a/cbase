#include "bconn.h"
#include "app_delay.h"


#define TEST_IP (192 << 24 | 168 <<16 | 251 <<8 | 170)
#define TEST_PORT 50003

typedef int nstar_handle;



struct _net_pg{
	nstar_handle fd;
	unsigned int lastTimeSec;
	unsigned int sndDtaCnt;
	unsigned int speedCnt;
	unsigned int speed;
	unsigned char keepSec;
	volatile unsigned char isNeedCreat;
};

#define FD_INVALID 0



static unsigned char rdata[2048];
static struct _net_pg g_netForward = {FD_INVALID,0,0};

static void __destroy_control(void)
{
	if(g_netForward.fd != FD_INVALID){
		close(g_netForward.fd);
		g_netForward.fd = FD_INVALID;
	}
}

static const char* sys_log_play(void)
{
	return 0;
}


static int creat_connect(unsigned int ip, unsigned short port)
{
    int conn_sock, rlen;
    struct sockaddr_in	server_addr;

    conn_sock	= socket(AF_INET, SOCK_STREAM , 0);
    if (conn_sock < 0) {
        perror("socket(2) error");
        return  -1;
    }

    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(port);
   	server_addr.sin_addr.s_addr = htonl(ip);
	while(1){

		if (connect(conn_sock,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) >= 0) {
	        perror("connect(2) ok");
	        break;
   		 }
		else{
			printf("wait connect to transmit addr %d.%d.%d.%d:%d\n",
					ip>>24, (ip&0xff0000)>>16, (ip&0xff00)>>8, ip&0xff, port);
			sleep(2);	
		}
	}
	

	printf("connect ok\n");
	return conn_sock;
}


static unsigned char _creat_connect_socket(void)
{
	int fd=0;
	unsigned int ip;
	unsigned short port;
	unsigned char ret = 0;
	__destroy_control();
	ip = TEST_IP;
	port = TEST_PORT;		
	if(ip > 0 && port > 0 ){
		fd = creat_connect(ip, port);
		if(fd  != FD_INVALID ){
			g_netForward.fd = fd;
			g_netForward.lastTimeSec = 0;
			g_netForward.keepSec = 0;
			ret = 1;	
		}
	}	
	return ret;
}


static void _forward_try_creat(void)
{
	if(g_netForward.isNeedCreat == 3){
		g_netForward.isNeedCreat = 0;
		__destroy_control();
	}
	else if(g_netForward.isNeedCreat == 1){
		if(1 == _creat_connect_socket()){
			g_netForward.isNeedCreat = 0;
		}
		else{
			g_netForward.isNeedCreat = 2;
			g_netForward.lastTimeSec = app_get_systime();
		}
	}
	else if(g_netForward.isNeedCreat == 2){
		if(g_netForward.lastTimeSec + 10 < app_get_systime()){
			if(1 == _creat_connect_socket()){
				g_netForward.isNeedCreat = 0;
			}
			else{
				g_netForward.lastTimeSec = app_get_systime();
			}
		}

	}
}

void nstar_net_forward_need_creat(void)
{
	g_netForward.isNeedCreat = 1;
}
void nstar_net_forward_need_destroy(void)
{
	g_netForward.isNeedCreat = 3;
}

void nstar_net_forward_recreat(void)
{
	__destroy_control();
	g_netForward.isNeedCreat = 2;
	g_netForward.lastTimeSec = app_get_systime() + 5;/*10 -5 =5s gaps*/
}

static void __check_data_status_clear(void)
{
//	unsigned int dtaLen = MP3_LEN;
//	while(1 == nstar_net_get(rdata,&dtaLen)){
//		dtaLen = MP3_LEN;
//	}
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
				nstar_net_forward_recreat();
			}
			else{
				printf("rlen %d bytes\n", rlen);
			}
				
		}
	}			
}


static void _check_data_status(void)
{

	block_rec(g_netForward.fd);
#if 0
	unsigned int sec = app_get_systime();
	if(g_netForward.lastTimeSec + 2 <  sec){
		static unsigned int print_ctl=0;
		g_netForward.speed= g_netForward.speedCnt/(sec -g_netForward.lastTimeSec);
		if((print_ctl++)%15 == 0)
			printf(" tcp snd speed sndDtaCnt:%d speed:%d byte/s\n",g_netForward.sndDtaCnt, g_netForward.speed );
		g_netForward.lastTimeSec = sec;
		g_netForward.speedCnt = 0;
	}
#endif
}


void nstar_net_forward_loop(void)
{
	_forward_try_creat();
	if(g_netForward.fd != FD_INVALID){
		_check_data_status();
	}
}



int main(int argc, char *argv[])
{
	nstar_net_forward_need_creat();
	while(1){
		nstar_net_forward_loop();
		usleep(10*1000);
	}
	return 0;
}




