#ifndef	__HTTPUTIL_H__
#define	__HTTPUTIL_H__

#include "nstar_adt_http_server.h"



int proc_http(SOCKET s, unsigned char * buf);
int do_https(SOCKET s);
void cgi_ipconfig(st_http_request *http_request, unsigned char mode, char* jump_to, unsigned char jumplen);
unsigned short make_msg_response(unsigned char* buf,char* msg);

void make_cgi_response(unsigned short delay, char* ip, char* jumptourl, char* cgi_response_buf);
void make_pwd_response(char isRight,unsigned short delay,char* cgi_response_content, char isTimeout);

#define ONLINE_WEB_UPDATE
#ifdef ONLINE_WEB_UPDATE
//#include "iap.h"
typedef struct __firmware{
	unsigned int fw_offset ; 				//写FLASH地址
	unsigned int current_rxlen;				//当前接收长度
    unsigned int total_rxlen;				//总的接收长度
    unsigned int total_w_len ;				//已写长度
    unsigned int real_wl; 					//真正写入FLASH的长度，需要4字节对齐
	unsigned int hdr_len;					//Content中，数据区的头部长度
	unsigned int data_len;					//Content中，数据区的长度
	unsigned int tail_len;					//Content中，数据区的尾部
	unsigned char  remain_buf[3];			//未4字节对齐的数据保存位置
    unsigned char  remain_len ; 			//未4字节对齐的数据长度
    unsigned char  head_flag;				//找到数据起始位置的标志
    char *pos1;
	char *pos2;
	unsigned char *pdata;
}WEB_FIRMWARE;

#define STMFLASH_Clear_BAK_sector()  
#endif

#endif


