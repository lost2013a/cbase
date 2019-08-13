#ifndef	__HTTPUTIL_H__
#define	__HTTPUTIL_H__

#include "nstar_adt_http_server.h"
#include "ntart_cookie.h"

extern char *nstar_web_tx_buf; 
extern char *nstar_web_rx_buf;

#define	JSON_START_SYMBOL	"({"	
#define	JSON_END_SYMBOL		"});"



int do_https(void);
void cgi_ipconfig(st_http_request *http_request, unsigned char mode, char* jump_to, unsigned char jumplen);
unsigned short make_msg_response(unsigned char* buf,char* msg);

void make_cgi_response(unsigned short delay, char* ip, char* jumptourl, char* cgi_response_buf);
void make_pwd_response(char isRight,unsigned short delay,char* cgi_response_content, char isTimeout);


int http_send(unsigned char *data, unsigned int len);

int http_rec(unsigned char *data, unsigned int rmax_len);


#define WEB_CMM_VARIABLE_CALC(func)	do{ offset = func;len += offset;p_offset += offset;}while(0)
				
void http_sprintf_init(void);
void http_sprintf(char* fmt,...);
void http_sprintf_send(void);


#endif


