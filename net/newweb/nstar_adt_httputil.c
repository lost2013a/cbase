#include <sys/socket.h>
#include "nstar_adt_httputil.h"
#include "nstar_adt_http_webpge.h"

extern char *nstar_web_tx_buf; 
extern char *nstar_web_rx_buf;
extern int conn_sock;

static int http_send(unsigned char *data, unsigned int len)
{
#define DEAFULT_FLAG 0
#if 1
#include "write_log.h"
	write_dbg(data, len,"[2]W");
#endif
	return send(conn_sock, data, len, DEAFULT_FLAG);
}

static int http_rec(unsigned char *data, unsigned int rmax_len)
{
#define DEAFULT_FLAG MSG_DONTWAIT
	int retlen;
	retlen= recv(conn_sock, data, rmax_len, DEAFULT_FLAG);
#if 1
#include "write_log.h"
	if(retlen > 1)
	write_dbg(data, retlen ,"[1]R");
#endif
	return retlen;
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

struct nstar_time{
    unsigned int seconds;
    unsigned int millisecond;
};

struct nstar_userpasswd{ 
#define NSTAR_HTTP_USER_LEN_MAX		(8)
#define NSTAR_HTTP_PASSWD_LEN_MAX	(10)

#define _USERPASSWD_LEN_MAX 15
#if NSTAR_HTTP_USER_LEN_MAX > _USERPASSWD_LEN_MAX | NSTAR_HTTP_PASSWD_LEN_MAX >   _USERPASSWD_LEN_MAX
	#param err!!
#endif
		unsigned char auth;
		unsigned char lenUser:4;
		unsigned char lenPasswd:4;
		unsigned char user[NSTAR_HTTP_USER_LEN_MAX];
		unsigned char passwd[NSTAR_HTTP_PASSWD_LEN_MAX];
};


struct http_cookie{
#define COOKIE_LEN	32
	unsigned char cookieLen;
	unsigned char cookie[COOKIE_LEN+1];
	struct nstar_userpasswd userpasswd;
	struct nstar_time time;
	unsigned int lastSec;
};

static struct http_cookie g_httpCookie = {0,};
#define WEB_CMM_COOKIE_HEAD 	"newstar"
#define WEB_CMM_COOKIE_HEAD_LEN	7
static unsigned char __check_cookie_by_respinse(unsigned char *p_dta)
{
	unsigned char ret = 0;
	char *p_cooki;
	unsigned int sec;
	p_cooki = strstr((const char *) p_dta, (const char *)WEB_CMM_COOKIE_HEAD);
	if(p_cooki != 0 && g_httpCookie.cookieLen > 0){
		mydbg(" __check_cookie_by_respinse %d %s\n",p_cooki[g_httpCookie.cookieLen+WEB_CMM_COOKIE_HEAD_LEN],p_cooki);
		p_cooki += WEB_CMM_COOKIE_HEAD_LEN;
		if(p_cooki[g_httpCookie.cookieLen] == '\r' && 0 == memcmp(p_cooki,g_httpCookie.cookie,g_httpCookie.cookieLen)){
			//sec = nstar_cmm_get_time_sec();
			sec = 1000;
			if(g_httpCookie.time.seconds + 	43200 < sec ||	6000 + g_httpCookie.lastSec < sec  ){
				g_httpCookie.cookieLen = 0;
			}
			else{
				ret = 1;
				g_httpCookie.lastSec = sec;
			}
		}
	}
	return ret;
}



void mid(char* src, char* s1, char* s2, char* sub)
{
	char* sub1;
	char* sub2;
	unsigned short n;
	sub1=strstr(src,s1);
	sub1+=strlen(s1);
	sub2=strstr(sub1,s2);
	n=sub2-sub1;
	strncpy(sub,sub1,n);
	sub[n]=0;
}

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



static void make_json_callback_config(char* buf)
{
	snprintf(buf,MAX_URI_SIZE,"json_callback_config({\
		\"lip\":\"%d.%d.%d.%d\",\
		\"sub\":\"%d.%d.%d.%d\",\
	    });",
	    config->local_ip[0],config->local_ip[1],config->local_ip[2],config->local_ip[3],
	    config->mask[0],config->mask[1],config->mask[2],config->mask[3]
	);
}



void make_cgi_noboot(char* ip, char* jumptourl, char* cgi_response_buf);


int do_https(SOCKET s)
{			
	int len;									
	st_http_request *http_request = (st_http_request*)nstar_web_rx_buf;
//	len	= recv(s, (unsigned char*)http_request, MAX_URI_SIZE,MSG_DONTWAIT);
	len	= http_rec((unsigned char*)http_request, MAX_URI_SIZE);
	if(len < 1)
		return 0;
	*(((unsigned char*)http_request)+len) = 0;
	proc_http(s, (unsigned char*)http_request);	
	return len; 
}


void pb_ipconfig(st_http_request *http_request);

static void repos_parm_htm(SOCKET s, unsigned char* http_response, const char* htm)
{
	char* pweb;
	unsigned long file_len=0;											/*定义http请求报文头的结构体指针*/
	unsigned short send_len=0;
	pweb= (char*)htm;
	file_len = strlen(pweb);
	make_http_response_head((unsigned char*)http_response, PTYPE_HTML,file_len);
	http_send(http_response, strlen((char const*)http_response));
	send_len=0;

#if 1	
	http_send((unsigned char *)pweb, file_len);
	
#else

	while(file_len)
	{
		if(file_len>1024)
		{
			http_send((unsigned char *)pweb+send_len, 1024);
			send_len+=1024;
			file_len-=1024;
		}
		else
		{
			http_send((unsigned char *)pweb+send_len, file_len);
			send_len+=file_len;
			file_len-=file_len;
		} 
	}


#endif
}




int proc_http(SOCKET s, unsigned char * buf)
{
	int ret=0;
	char* name; 											
	char req_name[32]={0x00,};											/*定义一个http响应报文的指针*/
	unsigned char* http_response;
	st_http_request *http_request;
	memset(nstar_web_tx_buf,0x00,MAX_URI_SIZE);
	http_response = (unsigned char*)nstar_web_rx_buf;
	http_request = (st_http_request*)nstar_web_tx_buf;

	if(0){
		unsigned char cookieStatus;
		cookieStatus = __check_cookie_by_respinse((unsigned char*)http_request);
	}
	
	parse_http_request(http_request, buf);    							/*解析http请求报文头*/

	switch (http_request->METHOD)		
 	{
		case METHOD_ERR :																			/*请求报文头错误*/
			memcpy(http_response, ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
			http_send((unsigned char *)http_response, strlen((char const*)http_response));
			break;
		
		case METHOD_HEAD:																			/*HEAD请求方式*/		
		case METHOD_GET:																			/*GET请求方式*/
			name = http_request->URI;
			if(strncmp(name,"/index.htm", strlen("/index.htm"))==0 || strcmp(name,"/")==0 ){
				repos_parm_htm(s, http_response, INDEX_HTML);
			}
			else if(strncmp(name,"/home.htm", strlen("/home.htm"))==0 )
			{
				repos_parm_htm(s, http_response, HOME_HTML);
			}	
			else if(strncmp(name,"/parm_1.htm", strlen("/parm_1.htm"))==0 )
			{
				repos_parm_htm(s, http_response, PARM_1_HTML);
			}
			else if(strncmp(name,"/parm_2.htm", strlen("/parm_2.htm"))==0 )
			{
				repos_parm_htm(s, http_response, PARM_2_HTML);
			}
			else if(strncmp(name,"/parm_3.htm", strlen("/parm_3.htm"))==0 )
			{
				repos_parm_htm(s, http_response, PARM_3_HTML);
			}
			else if(strncmp(name,"/parm_4.htm", strlen("/parm_4.htm"))==0 )
			{
				repos_parm_htm(s, http_response, PARM_4_HTML);
			}
			else if(strncmp(name,"/config.js", strlen("/config.js"))==0 )
			{
				memset(nstar_web_tx_buf,0,MAX_URI_SIZE);
				make_json_callback_config(nstar_web_tx_buf);
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);
				http_send((unsigned char *)http_response, strlen((char const*)http_response));
			}
			else if(strncmp(name,"/pb.js", strlen("/pb.js"))==0 )
			{
				memset(nstar_web_tx_buf,0,MAX_URI_SIZE);
				make_json_callback_cps(nstar_web_tx_buf);
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);
				http_send((unsigned char *)http_response, strlen((char const*)http_response));
			}
			else if(strncmp(name,"/sta.php", strlen("/sta.php"))==0 )
			{
				memset(nstar_web_tx_buf,0,MAX_URI_SIZE);
				make_json_callback_sta(nstar_web_tx_buf);
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);
				http_send((unsigned char *)http_response, strlen((char const*)http_response));
			}
			break;
		case METHOD_POST:		
			
			mid(http_request->URI, "/", " ", req_name);	
			if(strcmp(req_name,"config.cgi")==0)							  	
			{
				char jumpto[20];unsigned char maxlen=20;
				cgi_ipconfig(http_request, 1, jumpto, maxlen);			/*重启并保存*/
				make_cgi_response(5, (char*)config->local_ip, jumpto, nstar_web_tx_buf);	
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);																				/*发送http响应*/
				http_send((unsigned char *)http_response, strlen((char const*)http_response));
			}
			else if(strcmp(req_name,"saveonly.cgi")==0)							  	
			{
				char jumpto[20];unsigned char maxlen=20;
				cgi_ipconfig(http_request, 0, jumpto, maxlen);			/*只保存*/
				make_cgi_noboot((char*)config->local_ip, jumpto, nstar_web_tx_buf);	
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);																				/*发送http响应*/
				http_send((unsigned char *)http_response, strlen((char const*)http_response));
			}
			else if(strcmp(req_name,"bootmode.cgi")==0)							  	
			{
				char jumpto[20];unsigned char maxlen=20;
				cgi_ipconfig(http_request, 0, jumpto, maxlen);			/*只保存*/
				make_cgi_noboot((char*)config->local_ip, jumpto, nstar_web_tx_buf);	
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_web_tx_buf),nstar_web_tx_buf);																				/*发送http响应*/
				http_send((unsigned char *)http_response, strlen((char const*)http_response));
			}
			break;
		default :
			break;
	}
	return ret;
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


