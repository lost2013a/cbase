#ifndef	__HTTPUTIL_H__
#define	__HTTPUTIL_H__

#include "nstar_adt_http_server.h"



int proc_http(SOCKET s, unsigned char * buf);
int do_https(SOCKET s);
void cgi_ipconfig(st_http_request *http_request, unsigned char mode, char* jump_to, unsigned char jumplen);
unsigned short make_msg_response(unsigned char* buf,char* msg);

void make_cgi_response(unsigned short delay, char* ip, char* jumptourl, char* cgi_response_buf);
void make_pwd_response(char isRight,unsigned short delay,char* cgi_response_content, char isTimeout);
#endif


