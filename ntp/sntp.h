#ifndef	__NSTAR_SNTP_CLIENT_H__
#define __NSTAR_SNTP_CLIENT_H__

#define VERSION_3           3
#define VERSION_4           4		/*default*/
#define MODE_CLIENT         3	
#define NTP_LI_UNNONE       0x11	/*default*/
#define NTP_LI_NO       	0x0
#define NTP_VN              VERSION_4   
#define NTP_MODE            MODE_CLIENT
#define NTP_DEFAULT_STRATUM 0
#define NTP_DEFAULT_POLL    4
#define NTP_DEFAULT_PREC    -6

#define NTP_HLEN            48
#define NTP_PORT            123

#define TIMEOUT             2
#define BUFSIZE             1500
#define JAN_1970            0x83aa7e80
#define JAN_2030	 		0x70DBD880 + JAN_1970

#define NTP_CONV_FRAC32(x)  (uint64_t) ((x) * ((uint64_t)1<<32))    
#define NTP_REVE_FRAC32(x)  ((double) ((double) (x) / ((uint64_t)1<<32)))   

#define NTP_CONV_FRAC16(x)  (uint32_t) ((x) * ((uint32_t)1<<16))    
#define NTP_REVE_FRAC16(x)  ((double)((double) (x) / ((uint32_t)1<<16)))    


#define USEC2FRAC(x)        ((uint32_t) NTP_CONV_FRAC32( (x) / 1000000.0 )) 
#define FRAC2USEC(x)        ((uint32_t) NTP_REVE_FRAC32( (x) * 1000000.0 )) 


#define NTP_LFIXED2DOUBLE(x)   \
((double) ( ntohl(((struct l_fixedpt *) (x))->intpart) - JAN_1970 + FRAC2USEC(ntohl(((struct l_fixedpt *) (x))->fracpart)) / 1000000.0 ))   


struct s_fixedpt {
    uint16_t    intpart;
    uint16_t    fracpart;
};

struct l_fixedpt {
    uint32_t    intpart;
    uint32_t    fracpart;
};

typedef struct 
{
	unsigned int seconds;
	unsigned int fraction;
}ntp_time;


struct ntphdr {
#if __BYTE_ORDER == __BID_ENDIAN
    unsigned int    ntp_li:2;
    unsigned int    ntp_vn:3;
    unsigned int    ntp_mode:3;
#endif
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int    ntp_mode:3;
    unsigned int    ntp_vn:3;
    unsigned int    ntp_li:2;
#endif
	unsigned char  	stratum;	//时钟等级
	unsigned char  	poll;		//两个连续NTP的轮询间隔
	signed char  	precision;	//系统时钟精度
	unsigned int 	rootdelay;	//本地到主参考时钟源的往返时间
	unsigned int 	dispersion;	//系统时钟相对于主参考时钟的最大误差
	unsigned int 	refid;		//参考时钟源的标识
    struct l_fixedpt    ntp_refts;
    struct l_fixedpt    ntp_orits;
    struct l_fixedpt    ntp_recvts;
    struct l_fixedpt    ntp_transts;
};





#endif
