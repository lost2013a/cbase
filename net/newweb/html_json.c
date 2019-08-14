#include <sys/socket.h>
#include <stdarg.h>
#include "nstar_adt_httputil.h"
#include "nstar_adt_http_webpge.h"

#include "htm_login.h"

#define HTTP_SENDBUF nstar_web_tx_buf

extern int conn_sock;

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
#define HTML_PTYPE_HEAD "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n"
	
void http_sprintf_send(void)
{
	unsigned char headbuf[100];
	int hend_len;
	hend_len= sprintf((char*)headbuf, HTML_PTYPE_HEAD, http_sprintf_len);
	http_send(headbuf, hend_len);
	http_send(http_sprintf_buf, http_sprintf_len);
}



void make_cgi_response(unsigned short delay, char* ip, char* jumptourl, char* cgi_response_buf)
{
  sprintf(cgi_response_buf,"<html><head><title>iWeb - Configuration</title><script language=javascript>j=%d;function func(){document.getElementById('delay').innerText=' '+j + ' ';j--;setTimeout('func()',1000);if(j==0)location.href='http://%d.%d.%d.%d%s';}</script></head><body onload='func()'>please wait for a while, the module will boot in<span style='color:red;' id='delay'></span> seconds.</body></html>"
  	,delay,ip[0],ip[1],ip[2],ip[3],jumptourl);
  return;
}

void make_cgi_noboot(char* ip, char* jumptourl, char* cgi_response_buf)
{
  sprintf(cgi_response_buf,"<html><head><title>iWeb - Configuration</title><script language=javascript>;function func(){location.href='http://%d.%d.%d.%d%s';}</script></head><body onload='func()'></body></html>"
  	,ip[0],ip[1],ip[2],ip[3],jumptourl);
  return;
}


typedef struct _CFG_TYPE{
	unsigned char save_head_flag;
	unsigned char local_ip[4];
	unsigned char gateway[4];
	unsigned char mask[4];
	unsigned char mac[6];
	unsigned char vol[8]; 		
	unsigned char spkchl;   		
	unsigned char language0;   	
	unsigned char phyid[6];			
	unsigned char logicid[6];	
	unsigned char remote_ip[4];
	unsigned char web_disable;		
	unsigned char dbg_lvl;			
	unsigned char dbg_cpu_usage;		
	unsigned char backlinght;
	unsigned char sms_replay;			
	unsigned short local_port[8];
	unsigned short remote_port;
	unsigned int user_passw;		
	unsigned short ch_time_out;		
	unsigned short reserve2;			
	unsigned short reserve3;			
	unsigned short reserve4;			
	unsigned int  save_end_flag;
}CFG_TYPE;	

static CFG_TYPE TEST_CFG={0};
static const CFG_TYPE *READ_CFG= &TEST_CFG;
#define config  READ_CFG

extern char BDTIMEBUF[16];
extern char HADWARE[16];



static void make_json_callback_sta(char* buf)
{	
	sprintf(buf,"{\
		\"sq1\":\"%d\",\
		\"sq2\":\"%d\",\
		\"sq3\":\"%d\",\
		\"sq4\":\"%d\",\
		\"sq5\":\"%d\",\
		\"sq6\":\"%d\",\
		\"sq7\":\"%d\",\
		\"step1\":\"%s\",\
		\"step2\":\"%s\",\
		\"step3\":\"%s\",\
		\"step4\":\"%s\",\
		\"step5\":\"%s\",\
		\"step6\":\"%s\",\
		\"step7\":\"%s\",\
		\"cpuuse\":\"%02d.%02d\",\
	    }",
	  	1, 2, 3, 4, 5, 6, 7,
		ch_sta_uft8[1],
		ch_sta_uft8[1],
		ch_sta_uft8[1],
		ch_sta_uft8[1],
		ch_sta_uft8[1],
		ch_sta_uft8[1],
		ch_sta_uft8[1],
		1, 0
	);
}


static void make_json_callback_cps(char* buf)
{
	snprintf(buf,MAX_URI_SIZE,"json_callback_pb({\
		\"ver1\":\"%s\",\
		\"ver2\":\"%s\",\
		\"ph1\":\"%s\",\
	    });",
	    "v1.0",
	    "v1.0",
	    "test\\r\\n"
	);
}




static void repos_parm_htm(unsigned char* http_response, const char* htm)
{
	unsigned int len=0;
	len+= strlen(HTML_PARM_HEAD);
	len+= strlen(htm);
	http_sprintf_init();
	http_sprintf(HTML_PTYPE_HEAD, len);
	http_sprintf("%s", HTML_PARM_HEAD);
	http_sprintf("%s",(char*)htm);
	http_sprintf_send();
}




#define REQUST_JSCRIPT_HEAD "<script>"\
"function $(id) { return document.getElementById(id); };"\
"function %s(o) {"

#define REQUST_JSCRIPT_ELEMENT_STR		"if ($('%s')) $('%s').value = o.%s;"
#define REQUST_JSCRIPT_ELEMENT(element) http_sprintf(REQUST_JSCRIPT_ELEMENT_STR, element, element, element);


#define REQUST_JSCRIPT_END_STR "};"\
"</script>"\
"<script type='text/javascript' src='%s'></script>"\
"<script>"\
"function savereboot(){document.getElementById('frmSetting').action='config.cgi'; document.getElementById('frmSetting').submit(); }"\
"function saveonly(){document.getElementById('frmSetting').action='saveonly.cgi'; document.getElementById('frmSetting').submit(); }"\
"</script>"
#define REQUST_JSCRIPT_END(js_name) 	http_sprintf(REQUST_JSCRIPT_END_STR, js_name);


static void requst_jscript(void)
{
	const char *name= JSON_NAME_P1;
	http_sprintf(REQUST_JSCRIPT_HEAD, name);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E1);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E2);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E3);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E4);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E5);
	REQUST_JSCRIPT_END(JSCRIPT_P1);
}


static void _repos_parm(unsigned char* http_response, const char* head, const char* htm, const char* jscript)
{
	http_sprintf_init();
	http_sprintf("%s%s%s", head, htm, jscript);
	requst_jscript();
	http_sprintf_send();
}

static void requst_jscript2(void)
{
	const char *name= JSON_NAME_P2;
	http_sprintf(REQUST_JSCRIPT_HEAD, name);
	REQUST_JSCRIPT_ELEMENT(JS_P2_E1);
	REQUST_JSCRIPT_ELEMENT(JS_P2_E2);
	REQUST_JSCRIPT_ELEMENT(JS_P2_E3);
	REQUST_JSCRIPT_ELEMENT(JS_P2_E4);
	REQUST_JSCRIPT_ELEMENT(JS_P2_E5);
	REQUST_JSCRIPT_ELEMENT(JS_P2_E6);
	REQUST_JSCRIPT_END(JSCRIPT_P2);
}

static void _repos_parm2(unsigned char* http_response, const char* head, const char* htm, const char* jscript)
{
	http_sprintf_init();
	http_sprintf("%s%s%s", head, htm, jscript);
	requst_jscript2();
	http_sprintf_send();
}


static void _repos_jsack_para1(void)
{
	const char *name= JSON_NAME_P1;
	http_sprintf_init();
	http_sprintf("%s%s", name, JSON_START_SYMBOL);
	http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E1, 192,1,1,170,40001 );
	http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E2, 192,1,1,1,51001);
	http_sprintf("'%s':'%d.%d.%d.%d',",JS_P1_E3, 255,255,255,0 );
	http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E4, 1,2,3,4,5,6 );
	http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E5, 1,2,3,4,5,6 );
	http_sprintf("%s", JSON_END_SYMBOL);
	http_sprintf_send();
}


static void _repos_jsack_para2(void)
{
	const char *name= JSON_NAME_P2;
	http_sprintf_init();
	http_sprintf("%s%s", name, JSON_START_SYMBOL);
	http_sprintf("'%s':'%d',",JS_P2_E1, 6691);
	http_sprintf("'%s':'%d',",JS_P2_E2, 103);
	http_sprintf("'%s':'%d',",JS_P2_E3, 50 );
	http_sprintf("'%s':'%d',",JS_P2_E4, 128 );
	http_sprintf("'%s':'%d',",JS_P2_E5, 48000 );
	http_sprintf("'%s':'%d',",JS_P2_E6, 1024 );
	http_sprintf("%s", JSON_END_SYMBOL);
	http_sprintf_send();
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
		repos_parm_htm(http_response, HOME_HTML);
	}	
	else if(_comp_uri(name, "parm_1.html"))
	{
		_repos_parm(http_response, HTML_PARM_HEAD, PARM_1_HTML, "");
	}
	else if(_comp_uri(name, JSCRIPT_P1))
	{
		_repos_jsack_para1();
	}
	else if(_comp_uri(name, "parm_2.html"))
	{
		_repos_parm2(http_response, HTML_PARM_HEAD, PARM_2_HTML, "");
	}
	else if(_comp_uri(name, JSCRIPT_P2))
	{
		_repos_jsack_para2();
	}
	else if(_comp_uri(name, "parm_3.html"))
	{
		_repos_parm(http_response, HTML_PARM_HEAD, PARM_3_HTML, HTML_PARM3_JSON);
	}
	else if(_comp_uri(name, "parm_4.html"))
	{
		_repos_parm(http_response, HTML_PARM_HEAD, PARM_4_HTML, "");
	}
	else if(_comp_uri(name, "pb.js"))
	{
		memset(nstar_web_tx_buf,0,MAX_URI_SIZE);
		make_json_callback_cps(nstar_web_tx_buf);
		sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);
		http_send((unsigned char *)http_response, strlen((char const*)http_response));
	}
	else if(_comp_uri(name, "sta.php"))
	{
		memset(nstar_web_tx_buf,0,MAX_URI_SIZE);
		make_json_callback_sta(nstar_web_tx_buf);
		sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);
		http_send((unsigned char *)http_response, strlen((char const*)http_response));
	}
	else if(_comp_uri(name, "favicon.ico")){
		;
	}	
	else if(_comp_uri(name,"\0") ||  _comp_uri(name,"index.htm")){
		repos_parm_htm(http_response, INDEX_HTML);
	}	
	

}

static void _repos_method_post(st_http_request     *http_request, unsigned char* http_response)
{
	char req_name[32]={0x00,};								
	http_mid(http_request->URI, "/", " ", req_name);
	if(strcmp(req_name,"log_in.cgi")==0){
		char jumpto[20];unsigned char maxlen=20;
		cgi_log_in(http_request, 1, jumpto, maxlen);
	}				
	else if(strcmp(req_name,"config.cgi")==0)							  	
	{
		char jumpto[20];unsigned char maxlen=20;
		cgi_ipconfig(http_request, 1, jumpto, maxlen);
		make_cgi_response(5, (char*)config->local_ip, jumpto, nstar_web_tx_buf);	
		sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);																				/*发送http响应*/
		http_send((unsigned char *)http_response, strlen((char const*)http_response));
	}
	else if(strcmp(req_name,"saveonly.cgi")==0)							  	
	{
		char jumpto[20];unsigned char maxlen=20;
		cgi_ipconfig(http_request, 0, jumpto, maxlen);
		make_cgi_noboot((char*)config->local_ip, jumpto, nstar_web_tx_buf);	
		sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);																				/*发送http响应*/
		http_send((unsigned char *)http_response, strlen((char const*)http_response));
	}
	else if(strcmp(req_name,"bootmode.cgi")==0)							  	
	{
		char jumpto[20];unsigned char maxlen=20;
		cgi_ipconfig(http_request, 0, jumpto, maxlen);
		make_cgi_noboot((char*)config->local_ip, jumpto, nstar_web_tx_buf);	
		sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);																				/*发送http响应*/
		http_send((unsigned char *)http_response, strlen((char const*)http_response));
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



void cgi_ipconfig(st_http_request *http_request, unsigned char mode , char* jump_to, unsigned char jumplen)
{ 
	char *parm_url;
	parm_url = (char*)get_param_url(http_request->URI, jump_to, jumplen);		/*获取修改后的IP地址*/
	if(parm_url == NULL){
		mydbg("parm url not found\r\n");
		return;
	}

}




