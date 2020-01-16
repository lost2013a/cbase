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



#define MCAST_PORT 1212
#define MCAST_ADDR "237.12.12.12"
#define BUFLEN 1460
unsigned  char buff[BUFLEN];
#define PLAT_NET_IF_NAME "eth0"


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


extern unsigned char plat_mp3_init(void);

extern unsigned char plat_mp3_play(unsigned char *data, unsigned int len);


#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

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


tmpIpAddr = inet_addr(param.destIpAddr);
isMultiCast = IS_MULTICAST(tmpIpAddr);


void p_rtpplay(void *parm)
{
	int s, rlen, err = -1;
	struct sockaddr_in local_addr;
	struct ip_mreq mreq;
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == -1)
	{
		printf("pg socket err\n");
		return;
	}
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(MCAST_PORT);
	err = bind(s,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
	if(err < 0)
	{
		printf("pg bind err\n");
		return;
	}
	
	mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);
	
	_getLocalIpAddr(&(mreq.imr_interface.s_addr), PLAT_NET_IF_NAME);
	//mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
	
	err = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
	if (err < 0)
	{
		printf("pg add multiaddr err\n");
		return;
	}

	plat_mp3_init();

	while(1)	
	{	
		rlen = recv(s, buff, BUFLEN, 0);
		
		if( rlen < 0){
			perror("pg recv err");
		}
		else if(rlen > 16){

			RTP_FIXED_HEADER *phead= (RTP_FIXED_HEADER *)buff;
			unsigned short p_seq=  ntohs(phead->seq_no);
			printf("seq:%x\n", p_seq);
			//write_file(&buff[16], rlen-16);
			plat_mp3_play(&buff[16], rlen-16);
		}
		usleep(5000);
	}
	setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
	close(s);
	return;
}





