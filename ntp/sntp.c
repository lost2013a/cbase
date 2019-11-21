#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <endian.h>

#include "sntp.h"

#if 1
#define dbg printf
#else
#define dbg(...) 
#endif
#define NTP_SERVER      "120.25.115.20"
#define NTP_LOOP_TIME   (10)	/*单位：秒*/
#define K_MS 1		

int get_ntp_packet(void *buf, size_t *size)
{
    struct ntphdr *ntp;
    struct timeval tv;

    if (!size || *size<NTP_HLEN)
        return -1;

    memset(buf, 0, *size);
    ntp = (struct ntphdr *) buf;
    ntp->ntp_li = NTP_LI_UNNONE;
    ntp->ntp_vn = NTP_VN;
    ntp->ntp_mode = NTP_MODE;
    ntp->stratum = NTP_DEFAULT_STRATUM;
    ntp->poll = NTP_DEFAULT_POLL;
    ntp->precision = NTP_DEFAULT_PREC;

    gettimeofday(&tv, NULL);
    ntp->ntp_transts.intpart = htonl(tv.tv_sec + JAN_1970);
    ntp->ntp_transts.fracpart = htonl(USEC2FRAC(tv.tv_usec));
    *size = NTP_HLEN;
    return 0;
}

double get_rrt(const struct ntphdr *ntp, const struct timeval *recvtv)
{
    double t1, t2, t3, t4;

    t1 = NTP_LFIXED2DOUBLE(&ntp->ntp_orits);
    t2 = NTP_LFIXED2DOUBLE(&ntp->ntp_recvts);
    t3 = NTP_LFIXED2DOUBLE(&ntp->ntp_transts);
    t4 = recvtv->tv_sec + recvtv->tv_usec / 1000000.0;

    return (t4 - t1) - (t3 - t2);
}


double get_offset(const struct ntphdr *ntp, const struct timeval *recvtv)
{
    double t1, t2, t3, t4;
    t1 = NTP_LFIXED2DOUBLE(&ntp->ntp_orits);
    t2 = NTP_LFIXED2DOUBLE(&ntp->ntp_recvts);
    t3 = NTP_LFIXED2DOUBLE(&ntp->ntp_transts);
    t4 = recvtv->tv_sec + recvtv->tv_usec / 1000000.0;
	//dbg("t1: %lf\nt2: %lf\nt3: %lf\nt4: %lf\n", t1, t2, t3, t4);
	return ((t2 - t1) + (t3 - t4)) / 2;
}

static char filter_sntp_b=0,filter_sntp_i=0;
static int myfilter(int *off,int d,char *b,char *i)
{
#define RANK_B 3	
#define RANK_I 3	
	int ret = 0;
	int absoff ;
	absoff = abs(*off);
	if(*b < RANK_B){
		if(*i < RANK_I){ 			
			if(absoff < d)
				(*i)++;	
			else if((*i)>0)
				(*i)--;
		}
		else {					
			if(absoff<(d)){
				if(*b>0){
					(*b)--;
				}
			}
			else {
				(*b)++; 		
				*off = 0; 		
			}
			ret=1;
		}
	}
	else{  						
		*i = 0;
		*b = 0;
	}
	dbg("off=%d, *i=%d, *b=%d\n",*off,*i,*b);
	return ret;
}

#define ADJUST_OFF 500 //ms
#define NSATR_SNTP_FILTER(off)	myfilter(off, ADJUST_OFF, &filter_sntp_b, &filter_sntp_i)

static void adjust_time(double off)
{
	struct timeval tv;
	int ret;
	int off_sec= (int) off;
	int off_usec= (off)*1000000;
	int cal_off= off_sec*1000+ off_usec/1000;
	dbg("raw off=%lf, cal off= %dms\n", off, cal_off);	
	NSATR_SNTP_FILTER(&cal_off);
#if 0
	/*尽可能减少系统时间修改*/
	if(cal_off > ADJUST_OFF || cal_off < -ADJUST_OFF)
#else
	/*尽可能对准*/
	if(cal_off != 0)
#endif		
	{
		gettimeofday(&tv, NULL);
		tv.tv_sec += (int) off;
		int off_u= (off)*1000000;
		tv.tv_usec += off_u;
		//dbg("sntp: adjust time %d.%d\n", tv.tv_sec, tv.tv_usec);
		ret= settimeofday(&tv, NULL);
        if (ret != 0) {
            printf("sntp: adjust time %ld.%ld error #%d ", tv.tv_sec, tv.tv_usec ,ret);
        }
	}

}

int main(int argc, char *argv[])
{
	char buf[BUFSIZE];
    size_t nbytes;
    int sockfd, maxfd1;
    struct sockaddr_in servaddr;
    fd_set readfds;
    struct timeval timeout, recvtv;
    double offset;
	
	
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(NTP_PORT);
    servaddr.sin_addr.s_addr = inet_addr(NTP_SERVER);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        exit(-1);
    }

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) != 0) {
        perror("connect error");
        exit(-1);
    }

    nbytes = BUFSIZE;
	
	while(1)
	{
	    if (get_ntp_packet(buf, &nbytes) != 0) {
	        fprintf(stderr, "construct ntp request error \n");
	        exit(-1);
	    }
	    send(sockfd, buf, nbytes, 0);
	
	    FD_ZERO(&readfds);
	    FD_SET(sockfd, &readfds);
	    maxfd1 = sockfd + 1;
	   	timeout.tv_sec = 3;
		timeout.tv_usec= 0;
		
		int ret = select(maxfd1, &readfds, NULL, NULL, &timeout);
		switch(ret)
		{
			case -1:
				perror("sntp select err!\n");
				break;
			case 0:
				printf("sntp wait sync msg time out!\n");
				break;
			default:	
		        if (FD_ISSET(sockfd, &readfds)) {
		            if ((nbytes = recv(sockfd, buf, BUFSIZE, 0)) < 0) {
		                perror("recv error");
		                exit(-1);
		            }
		            gettimeofday(&recvtv, NULL);
		            offset = get_offset((struct ntphdr *) buf, &recvtv);
					adjust_time(offset);				
		        }
				break;
	    }
		sleep(NTP_LOOP_TIME);
	}
    close(sockfd);
    return 0;
}
