#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#define MCAST_PORT 50001
#define MCAST_ADDR "237.0.0.3"
#define BUFLEN 1460
char buff[BUFLEN];

static rec_ts(int len, unsigned char *buf)
{

	static unsigned short last_pid;
	static unsigned char s_tsCount ;
	unsigned int tsCount=0;
	unsigned short pid,audioPid,cal=0;
	int i=0;
	audioPid= 6501;
	unsigned char *p_dta= buf;

	for(i=0;i< 188;i++){
		cal+=p_dta[i];
	}	

	if(p_dta[0] == 0x47 ){
		pid = (p_dta[1] << 8 | p_dta[2])&0x1fff;
		
		if(pid != audioPid){
			p_dta+=188;
			len -= 188;
			return;
		}
		tsCount = p_dta[3]&0xf;

		printf("len %d[%02d]: ",len, tsCount);
		for(i=0;i< 20;i++){
			printf("%02x ", p_dta[i]);

		}	
		for(i=0;i< 188;i++){
			cal+=p_dta[i];
		}	
		printf("cal =%d \r\n", cal);	
			//s_tsCount = (tsCount+1)%0x10;	

			
	}
	else{
		return;
	}
	p_dta+=188;
	len -= 188;
	printf("\n");
}
int main(int argc, char*argv[])
{
	int s, rlen, err = -1;
	struct sockaddr_in local_addr;
	struct ip_mreq mreq;
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == -1)
	{
		perror("pg socket err");
		return -1;
	}
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(MCAST_PORT);
	err = bind(s,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
	if(err < 0)
	{
		perror("pg bind err");
		return -2;
	}
	mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	err = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(mreq));
	if (err < 0)
	{
		perror("pg add multiaddr err");
		return -3;
	}
	while(1)	
	{	
		rlen = recv(s, buff, BUFLEN, 0);	
		if( rlen < 0){
			perror("pg recv err");
		}
		else if(rlen > 1){
			
			rec_ts(rlen, buff);
			memset(buff, 0, BUFLEN);
			//printf("Recv%d\n", rlen);
		}
		usleep(5000);
	}
	setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof(mreq));
	close(s);
	return 0;
}





