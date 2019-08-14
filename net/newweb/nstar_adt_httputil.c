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




static unsigned char GET_comp_uri(const char* uri, const char* str)
{

	unsigned int len= strlen(str);
	unsigned char ret=URI_REPOS_UNVAILD;
	if(len == 0 || uri[0]!='/')
		return ret;
	uri++;
	if(0 == strncmp(uri, str, len)){
		if(0 == strncmp(&uri[len], URI_HTML, strlen(URI_HTML)))
			ret= URI_REPOS_HTML;
		else if(0 == strncmp(&uri[len], URI_JSON, strlen(URI_JSON)))
			ret= URI_REPOS_JSON;
	}
	return ret;
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
	unsigned char mode=0;
	if((mode= GET_comp_uri(name, HTML_PAGE1_NAME) ) > 0)
		parm1_pos_htm(mode);
	if((mode= GET_comp_uri(name, HTML_PAGE2_NAME) ) > 0)
		parm2_pos_htm(mode);


}


static void make_cgi_noboot(char* ip, char* jumptourl, char* cgi_response_buf)
{
  sprintf(cgi_response_buf,"<html><head><title>iWeb - Configuration</title><script language=javascript>;function func(){location.href='http://%s%s';}</script></head><body onload='func()'></body></html>"
  	,ip,jumptourl);
  return;
}
		


static void _repos_method_post(st_http_request     *http_request, unsigned char* http_response)
{
	char req_name[32]={0x00,};			
	char jumpto[20];unsigned char maxlen=20;
	http_mid(http_request->URI, "/", " ", req_name);
	if(strcmp(req_name,"log_in.cgi")==0){
	}				
	else if(strcmp(req_name,""HTML_PAGE1_NAME".cgi")==0)							  	
	{
		char jumpto[20];unsigned char maxlen=20;
		parm1_rpos_cgi(http_request, 0, jumpto, maxlen);			/*只保存*/
		make_cgi_noboot((char*)"192.168.251.175", jumpto, nstar_web_tx_buf);	
		sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);				
		http_send((unsigned char *)http_response, strlen((char *)http_response));
	}
	else if(strcmp(req_name,""HTML_PAGE2_NAME".cgi")==0)							  	
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




void http_page_htm(const char *name, const char *body, void (fun_add_elemnet)(unsigned char))
{
	http_sprintf_init();
	http_sprintf("%s%s", HTML_PARM_HEAD, body);
	http_sprintf(REQUST_JSCRIPT_HEAD, name);
	fun_add_elemnet(URI_REPOS_HTML);
	REQUST_JSCRIPT_END(name);
	http_sprintf_send();
}

void http_page_json(const char *name, void (fun_add_elemnet)(unsigned char))
{
	http_sprintf_init();
	http_sprintf("json_%s%s", name, JSON_START_SYMBOL);
	fun_add_elemnet(URI_REPOS_JSON);
	http_sprintf(JSON_END_SYMBOL);
	http_sprintf_send();
}





