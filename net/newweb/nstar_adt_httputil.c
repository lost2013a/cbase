#include <sys/socket.h>
#include <stdarg.h>
#include "nstar_adt_httputil.h"
#include "htm_login.h"

#include "nstar_adt_http_page.h"

#define HTTP_SENDBUF nstar_web_tx_buf

extern int conn_sock;

#define HTML_PTYPE_HEAD "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n"
	


#define SOCKET_SND_FLAG 0
#define SOCKET_REC_FLAG MSG_DONTWAIT

int http_send(unsigned char *data, unsigned int len)
{
#if 1
#include "write_log.h"
	log_write(data, len,"[2]W");
#endif
	return send(conn_sock, data, len, SOCKET_SND_FLAG);
}

int http_rec(unsigned char *data, unsigned int rmax_len)
{
	int retlen;
	retlen= recv(conn_sock, data, rmax_len, SOCKET_REC_FLAG);
#if 1
#include "write_log.h"
	if(retlen > 1)
	log_write(data, retlen ,"[1]R");
#endif
	return retlen;
}

volatile static unsigned int http_sprintf_len;
#define HTTP_SPRINTF_MAXLEN 8000
static unsigned char http_sprintf_buf[HTTP_SPRINTF_MAXLEN];

void http_sprintf_init(void)
{
	http_sprintf_len=0;
}
void http_sprintf(char* fmt,...)  
{  
	volatile unsigned int *len= &http_sprintf_len;
	unsigned char* pbuf = (unsigned char*)http_sprintf_buf;
	va_list ap;
	va_start(ap,fmt);
	*len+= vsnprintf((char*)&pbuf[*len], HTTP_SPRINTF_MAXLEN - *len, fmt,ap);
	if(*len >= HTTP_SPRINTF_MAXLEN)
		printf("http printf maybe flow out\n");
	va_end(ap);
}

void http_sprintf_addhead(void)
{
	http_send(http_sprintf_buf, http_sprintf_len);
}

void http_sprintf_send(void)
{
	unsigned char headbuf[100];
	int hend_len;
	hend_len= sprintf((char*)headbuf, HTML_PTYPE_HEAD, http_sprintf_len);
	http_send(headbuf, hend_len);
	http_send(http_sprintf_buf, http_sprintf_len);
}




static unsigned char _comp_uri(const char* uri, const char* str)
{
	if(uri[0]!='/')
		return 0;
	else{
		uri+=1;
		return (0 == strncmp(uri,str,strlen(str)))? 1: 0;
	}
}


static void _repos_method_get(st_http_request     *http_request, unsigned char* http_response)
{
	char *name= http_request->URI;
	if(_comp_uri(name, "home.html"))
	{
	}	
	else if(_comp_uri(name, "parm_1.html"))
	{
		parm1_pos_htm();
	}
	else if(_comp_uri(name, ""HTML_PAGE1_NAME".js"))
	{
		parm1_pos_json();
	}
	else if(_comp_uri(name, "parm_1.html"))
	{
	}
	else if(_comp_uri(name, "parm_2.html"))
	{
	}
	

}

static void _repos_method_post(st_http_request     *http_request, unsigned char* http_response)
{
	char req_name[32]={0x00,};								
	http_mid(http_request->URI, "/", " ", req_name);
	if(strcmp(req_name,"log_in.cgi")==0){
	}				
	else if(strcmp(req_name,"config.cgi")==0)							  	
	{
	}
	else if(strcmp(req_name,"saveonly.cgi")==0)							  	
	{
	}
	else if(strcmp(req_name,"bootmode.cgi")==0)							  	
	{
	}
}

static void proc_http(void)
{								
	unsigned char* http_response;
	st_http_request *http_request;
	http_response = (unsigned char*)nstar_web_rx_buf;
	http_request = (st_http_request*)nstar_web_tx_buf;
	memset(nstar_web_tx_buf,0x00,MAX_URI_SIZE);
	if(1){
		cookie_verify((unsigned char*)http_response);
	}
	parse_http_request(http_request, (unsigned char*)nstar_web_rx_buf);
	switch (http_request->METHOD)	
 	{
		case METHOD_ERR:
			http_send((unsigned char*)ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
			break;
		case METHOD_HEAD:			
		case METHOD_GET:	
			_repos_method_get(http_request, http_response);
			break;
		case METHOD_POST:
			_repos_method_post(http_request, http_response);
			break;
		default:
			break;
	}
}

int do_https(void)
{			
	int len;									
	len	= http_rec((unsigned char*)nstar_web_rx_buf, MAX_URI_SIZE);
	if(len < 1)
		return 0;
	nstar_web_rx_buf[len] = 0;
	proc_http();	
	return len; 
}






