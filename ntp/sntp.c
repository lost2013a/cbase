
/* ntpclient.c */
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

#define NTP_SERVER          "148.70.110.182"
#define K_MS 1		


in_addr_t inet_host(const char *host)
{
    in_addr_t saddr;
    struct hostent *hostent;

    if ((saddr = inet_addr(host)) == INADDR_NONE) {
        if ((hostent = gethostbyname(host)) == NULL)
            return INADDR_NONE;

        memmove(&saddr, hostent->h_addr, hostent->h_length);
    }

    return saddr;
}

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
	//printf("t1:%lf t2: %lf t3:%lf t4:%lf \n\n", t1, t2, t3, t4);
	
	return ((t2 - t1) + (t3 - t4)) / 2;
	
}




int main(int argc, char *argv[])
{

	static unsigned int rcnt=5;
	static double last_time;
	
	char buf[BUFSIZE];
    size_t nbytes;
    int sockfd, maxfd1;
    struct sockaddr_in servaddr;
    fd_set readfds;
    struct timeval timeout, recvtv, tv;
    double offset;

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(NTP_PORT);
    servaddr.sin_addr.s_addr = inet_host(NTP_SERVER);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        exit(-1);
    }

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) != 0) {
        perror("connect error");
        exit(-1);
    }

    nbytes = BUFSIZE;
	
	while(rcnt--)
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
					
					printf("off= %lfus,  jitter= %lfus\n\n", offset /K_MS, (offset- last_time)/K_MS);
					last_time= offset;
					gettimeofday(&tv, NULL);
					tv.tv_sec += (int) offset;
					int off_u= (offset)*1000000;
					tv.tv_usec += off_u;
		            if (settimeofday(&tv, NULL) != 0) {
		                perror("settimeofday error");
		                exit(-1);
		            }
		        }
				break;
	    }
		sleep(1);
	}
    close(sockfd);
    return 0;
}
