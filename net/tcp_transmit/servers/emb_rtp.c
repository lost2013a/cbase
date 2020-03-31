#include "ebm_env.h"

#if 0
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#endif

#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>



#define MCAST_PORT 1212
#define MCAST_ADDR "237.12.12.12"
#define BUFLEN 1460
unsigned  char buff[BUFLEN];



#define __PACKED__        __attribute__ ((__packed__)) 

struct _RTP_FIXED_HEADER
{
  
    unsigned char csrc_len:4;        /**//* expect 0 */
	unsigned char extension:1;        /**//* expect 1, see RTP_OP below */
	unsigned char padding:1;        /**//* expect 0 */
	unsigned char version:2;        /**//* expect 2 */
	
    /**//* byte 0 */
    /**//* bytes 2, 3 */
    /**//* byte 1 */
    
    unsigned char payload:7;        /**//* RTP_PAYLOAD_RTSP */
	unsigned char marker:1;        /**//* expect 1 */
	
    unsigned short seq_no;            
    /**//* bytes 4-7 */
    unsigned int timestamp;        
    /**//* bytes 8-11 */
    unsigned int ssrc;            /**//* stream number is used here. */
} __PACKED__;

typedef struct _RTP_FIXED_HEADER RTP_FIXED_HEADER;

#if 0
extern unsigned char plat_mp3_init(void);

extern unsigned char plat_mp3_play(unsigned char *data, unsigned int len);
#else

#define plat_mp3_init()
#define plat_mp3_play(x,y) 

#endif




void _getLocalIpAddr(in_addr_t *addr, char* ifa_name)
{
    struct ifaddrs *ifAddrStruct = NULL;
    getifaddrs(&ifAddrStruct);
    while (ifAddrStruct != NULL)
    {
       if (ifAddrStruct->ifa_addr->sa_family == AF_INET)
       {
           if (strcmp(ifa_name,ifAddrStruct->ifa_name) == 0)
           {
               memcpy(addr, &(((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr.s_addr), sizeof(in_addr_t));
           }
       }
       ifAddrStruct = ifAddrStruct->ifa_next;
    }
    return;
}


#define IS_MULTICAST(a)    ((((unsigned long)(a)) & 0x000000F0) == 0x000000E0)


static struct bsockt rtp_net_handle = {FD_INVALID,0,0};





static void _socket_close(int *fd)
{
	if(*fd != FD_INVALID){
		close(*fd);
		*fd = FD_INVALID;
	}
}


static int _socket_open_unicast(unsigned int server_ip, unsigned short server_port)
{
	int conn_sock;
	struct sockaddr_in server_addr;
	
	conn_sock	= socket(AF_INET, SOCK_DGRAM, 0);
	if (conn_sock < 0) {
		perror("socket(2) error");
		return FD_INVALID;	
	}

	(void)memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family		= AF_INET;
	server_addr.sin_port		= htons(server_port);
	
	if (bind(conn_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
		perror("bind(2) error");
		goto release;
	}
	
#if 1	
	/*test "0" byte */
	server_addr.sin_addr.s_addr	= htonl(server_ip);
	if (sendto(conn_sock, "0", 0 , 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("send data error");
        goto release;
    }
#endif	

	return conn_sock;
release:
	close(conn_sock);
	return FD_INVALID;	
}

static int _socket_open_multicast(unsigned int macst_ip, unsigned short mcast_port)
{
	int conn_sock, err = -1;
	struct sockaddr_in local_addr;
	struct ip_mreq mreq;
	conn_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (conn_sock == -1)
	{
		printf("pg socket err\n");
		return FD_INVALID;
	}
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(mcast_port);
	err = bind(conn_sock,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
	if(err < 0)
	{
		printf("pg bind err\n");
		goto release;
	}
	mreq.imr_multiaddr.s_addr = htonl(macst_ip);
#if 0	/*嵌入式*/
#define PLAT_NET_IF_NAME "eth0"
	_getLocalIpAddr(&(mreq.imr_interface.s_addr), PLAT_NET_IF_NAME);
#else
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
#endif
	setsockopt(conn_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
	
	err = setsockopt(conn_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
	if (err < 0)
	{
		printf("pg add multiaddr err\n");
		goto release;;
	}
	return conn_sock;
release:
	close(conn_sock);
	return FD_INVALID;
}


static unsigned char _creat_connect_socket(struct bsockt *h_net)
{
	int fd=0;
	unsigned int ip;
	unsigned short port;
	unsigned char ret = 0;
	_socket_close(&h_net->fd);

	ip = h_net->ipaddr;
	port = h_net->port ;	


	
	if(ip > 0 && port > 0 ){
		unsigned int tmpip= htonl(ip);
		if(IS_MULTICAST(tmpip)){
			printf("it's a muliticast ipaddr: %x:%d\n", ip, port);
			fd = _socket_open_multicast(ip, port);
		}
		else{
			printf("it's a unicast ipaddr: %x:%d\n", ip, port);
			fd = _socket_open_unicast(ip, port);
		}
		if(fd  != FD_INVALID ){
			h_net->fd = fd;
			h_net->lastTimeSec = 0;
			h_net->keepSec = 0;
			ret = 1;	
		}
	}	
	return ret;
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
#if 1

			memset(rbuf, 0 , 20);
#endif
		
			int rlen = read(fd, rbuf, 1460);
			if(rlen < 0){
				printf("rtp socket unvaild\n");
				command_net_restart();
			}
			else{

				static unsigned short last_sn=0;
				unsigned short new_sn;
				new_sn= (rbuf[2] << 8) | rbuf[3];

				if(new_sn == last_sn)
					printf("= sn %d\n", last_sn);
				if(rlen == 400 && new_sn != last_sn){
					plat_mp3_play(&rbuf[16], rlen-16);	
					if(new_sn != last_sn+1)
						printf("! sn= %03d / %03d\n", new_sn, last_sn);
					last_sn= new_sn;
				}
				
				
			}
				
		}
	}			
}


static void new_block_rec(unsigned int fd)
{
#define BUFF_SIZE	(1024 * 4)

	unsigned char 	buff[BUFF_SIZE] = {0};
	ssize_t			   len			   = 0;
	struct sockaddr_in  source_addr;
	socklen_t   addr_len    = sizeof(source_addr);

	(void)memset(&source_addr, 0, addr_len);
	len = recvfrom(fd, buff, BUFF_SIZE, 0,
				  (struct sockaddr *)&source_addr, &addr_len);

	printf("r %d , Served client %s:%hu\n",len, inet_ntoa(source_addr.sin_addr),ntohs(source_addr.sin_port));
	if(0){
		int i;
		for(i=0;i<20;i++)
			printf("%02x ", buff[i]);
		printf("\n");

	}

}



static void data_process(void)
{
	block_rec(rtp_net_handle.fd);
}


static void rtp_net_machine(void *parm)
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

void rtp_net_start(unsigned int remote_ip, unsigned short remote_port)
{
	struct bsockt *h_net= &rtp_net_handle;
	h_net->isNeedCreat = 1;
	h_net->ipaddr= remote_ip;
	h_net->port= remote_port;
}
void rtp_net_stop(void)
{
	struct bsockt *h_net= &rtp_net_handle;
	h_net->isNeedCreat = 3;
}

void rtp_net_restart(void)
{
	struct bsockt *h_net= &rtp_net_handle;
	_socket_close(&h_net->fd);
	h_net->isNeedCreat = 2;
	h_net->lastTimeSec = app_get_systime() + 5;/*10 -5 =5s gaps*/
}


void rtp_net_loop(void)
{
	rtp_net_machine(&rtp_net_handle);
	if(rtp_net_handle.fd != FD_INVALID){
		data_process();
	}
	usleep(10*1000);
}







