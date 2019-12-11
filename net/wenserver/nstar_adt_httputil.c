#include <sys/socket.h>
#include <stdarg.h>
#include "nstar_adt_httputil.h"
#include "nstar_adt_http_page.h"

#define HTTP_SENDBUF nstar_web_tx_buf

extern int conn_sock;

#define HTML_PTYPE_HEAD "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n"
	
#define SOCKET_SND_FLAG 0
#define SOCKET_REC_FLAG MSG_DONTWAIT

volatile static unsigned int http_data_len;
static int raw_content_len;
#define HTTP_SPRINTF_MAXLEN (1024*100)
static unsigned char http_data_buf[HTTP_SPRINTF_MAXLEN];

int http_send(unsigned char *data, unsigned int len)
{
#if 1
#include "write_log.h"
	log_write(data, len,"[2]W");
#endif
	return send(conn_sock, data, len, SOCKET_SND_FLAG);
}

static int http_rec(unsigned char *data, unsigned int rmax_len)
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
int test_http_rec(unsigned char *data, unsigned int rmax_len)
{
	return http_rec(data, rmax_len);
}


void http_sprintf_init(void)
{
	http_data_len=0;
}
void http_sprintf(char* fmt,...)  
{  
	volatile unsigned int *len= &http_data_len;
	unsigned char* pbuf = (unsigned char*)http_data_buf;
	va_list ap;
	va_start(ap,fmt);
	*len+= vsnprintf((char*)&pbuf[*len], HTTP_SPRINTF_MAXLEN - *len, fmt,ap);
	if(*len >= HTTP_SPRINTF_MAXLEN)
		printf("http printf maybe flow out\n");
	va_end(ap);
}

void http_sprintf_addhead(void)
{
	http_send(http_data_buf, http_data_len);
}

void http_sprintf_send(void)
{
	unsigned char headbuf[100];
	int hend_len;
	hend_len= sprintf((char*)headbuf, HTML_PTYPE_HEAD, http_data_len);
	http_send(headbuf, hend_len);
	http_send(http_data_buf, http_data_len);
}

//void http_raw_send(unsigned int len)
//{
//	http_send(http_data_buf, len);
//}

void http_raw_send(unsigned char *data, unsigned int len)
{
	memcpy(http_data_buf, data, len);
	http_send(http_data_buf, len);
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
	return (0 == strncmp(uri,str,strlen(str)))? 1: 0;
}


static void _repos_method_get(st_http_request     *http_request, unsigned char* http_response)
{
	char *name= http_request->URI;
	unsigned char mode=0;
	if(0 == memcmp(name, "/\0", 2) || _comp_uri(name, "/"HTML_PAGE0_NAME"") ){
		parm0_pos_htm(mode);
		return;
	}
	if(1 != cookie_verify((unsigned char*)http_response))
		verify_time_out();
	else if((mode= GET_comp_uri(name, HTML_PAGE1_NAME) ) > 0)
		parm1_pos_htm(mode);
	else if((mode= GET_comp_uri(name, HTML_PAGE2_NAME) ) > 0)
		parm2_pos_htm(mode);
	else if((mode= GET_comp_uri(name, HTML_PAGE3_NAME) ) > 0)
		parm3_pos_htm(mode);
	else if((mode= GET_comp_uri(name, HTML_PAGE4_NAME) ) > 0)
		parm4_pos_htm(mode);
	else if((mode= GET_comp_uri(name, HTML_PAGE5_NAME) ) > 0)
		parm5_pos_htm(mode);
	else if(_comp_uri(name, "/logo.png")){
		printf("get logo\n");
		parm5_pos_pic(0);
	}
	else if(_comp_uri(name, "/weixin.gif")){
		printf("get weixin\n");
		parm5_pos_pic(1);
	}
	else if(_comp_uri(name, "/png1.png")){
		printf("get png1.png\n");
		parm5_pos_pic(2);
	}
	else if(_comp_uri(name, "/png2.png")){
		printf("get png2\n");
		parm5_pos_pic(3);
	}
	else if(_comp_uri(name, "/png3.png")){
		printf("get png3\n");
		parm5_pos_pic(4);
	}
	else if(_comp_uri(name, "/png4.png")){
		printf("get png4\n");
		parm5_pos_pic(5);
	}
	else if(_comp_uri(name,"/favicon.ico")){
		/*nothing need to do*/
	}

}


static void _repos_method_post(st_http_request     *http_request, unsigned char* http_response)
{
	char req_name[32]={0x00,};			
	char *uri= http_request->URI;
	if(NULL == http_mid(uri, "/", " ", req_name))
		return;
	if(strcmp(req_name,""HTML_PAGE0_NAME".cgi")==0){
		parm0_rpos_cgi(uri);	
		return;
	}
	if(1 != cookie_verify((unsigned char*)http_response))
		verify_time_out();			
	else if(strcmp(req_name,""HTML_PAGE1_NAME".cgi") == 0)
		;						  	
		//parm1_rpos_cgi(uri);
	else if(strcmp(req_name,""HTML_PAGE2_NAME".cgi")==0)							  	
		parm2_rpos_cgi(uri);
	else if(strcmp(req_name,""HTML_PAGE3_NAME".cgi")==0)							  	
		parm3_rpos_cgi(uri);
	else if(strcmp(req_name,""HTML_PAGE4_NAME".cgi")==0)							  	
		parm4_rpos_cgi(uri);
	else if(strcmp(req_name,""HTML_PAGE5_NAME".cgi")==0)							  	
		parm5_rpos_cgi(uri);

}



static void proc_http(void)
{								
	unsigned char* http_response;
	st_http_request *http_request;
	http_response = (unsigned char*)nstar_web_rx_buf;
	http_request = (st_http_request*)nstar_web_tx_buf;
	memset(http_request,0x00,MAX_URI_SIZE);
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
	raw_content_len= len;
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

void http_handle_parm(char *p_content, char *name, void(fun_set_parm)(char*))
{
	char *p_parm= get_http_param_value((unsigned char*)nstar_web_rx_buf, p_content, name);		
	if(p_parm)
	{	
		fun_set_parm(p_parm);
	}
}

/*仅供web file计算file第一帧包内容长度时使用*/
char* web_file_frame1_content_len(unsigned int *content_len)
{
	char *p_content= (char*)my_get_param_url(nstar_web_rx_buf + 10);
	if(NULL == p_content){
		*content_len= 0;
	}
	else{
		*content_len= raw_content_len- (p_content- nstar_web_rx_buf);
	}
	return p_content;
}




