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


#define HTML_PTYPE_HEAD "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n"
	


#define REQUST_JSCRIPT_HEAD "<script>"\
"function $(id) { return document.getElementById(id); };"\
"function %s.json(o) {"

#define REQUST_JSCRIPT_ELEMENT_STR		"if ($('%s')) $('%s').value = o.%s;"
#define REQUST_JSCRIPT_ELEMENT(element) http_sprintf(REQUST_JSCRIPT_ELEMENT_STR, element, element, element);


#define REQUST_JSCRIPT_END_STR "};"\
"</script>"\
"<script type='text/javascript' src='%s.js'></script>"\

#define REQUST_JSCRIPT_END(name) 	http_sprintf(REQUST_JSCRIPT_END_STR, name);



#endif


