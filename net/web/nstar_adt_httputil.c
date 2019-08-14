#include "nstar_adt_httputil.h"
#include "nstar_adt_http_webpge.h"
#include "nstar_adt_test.h"

#define SOCKET_SEND_FLAG 0


//extern char *nstar_adt_web_tbuf; 
//extern char *nstar_adt_web_rbuf;

//试例
extern char BDTIMEBUF[16];
extern char HADWARE[16];

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



static void mid(char* src, char* s1, char* s2, char* sub)
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


void do_https(SOCKET s)
{													
	unsigned short len;									
	st_http_request *http_request = (st_http_request*)nstar_adt_web_rbuf;
	len	= read(s, (unsigned char*)http_request, MAX_URI_SIZE);
	if(len < 1)
		return;
	*(((unsigned char*)http_request)+len) = 0;
	proc_http(s, (unsigned char*)http_request);					
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
	send(s,http_response,strlen((char const*)http_response), SOCKET_SEND_FLAG);
	send_len=0;
	while(file_len)
	{
		if(file_len>1024)
		{
			send(s, (unsigned char *)pweb+send_len, 1024, SOCKET_SEND_FLAG);
			send_len+=1024;
			file_len-=1024;
		}
		else
		{
			send(s, (unsigned char *)pweb+send_len, file_len, SOCKET_SEND_FLAG);
			send_len+=file_len;
			file_len-=file_len;
		} 
	}


}

void proc_http(SOCKET s, unsigned char * buf)
{
	char* name; 											
	char req_name[32]={0x00,};											/*定义一个http响应报文的指针*/
	unsigned char* http_response;
	st_http_request *http_request;
	memset(nstar_adt_web_tbuf,0x00,MAX_URI_SIZE);
	http_response = (unsigned char*)nstar_adt_web_rbuf;
	http_request = (st_http_request*)nstar_adt_web_tbuf;
	parse_http_request(http_request, buf);    							/*解析http请求报文头*/

	switch (http_request->METHOD)		
  {
		case METHOD_ERR :																			/*请求报文头错误*/
			memcpy(http_response, ERROR_REQUEST_PAGE, sizeof(ERROR_REQUEST_PAGE));
			send(s, (unsigned char *)http_response, strlen((char const*)http_response), SOCKET_SEND_FLAG);
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
				memset(nstar_adt_web_tbuf,0,MAX_URI_SIZE);
				make_json_callback_config(nstar_adt_web_tbuf);
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_adt_web_tbuf),nstar_adt_web_tbuf);
				send(s, (unsigned char *)http_response, strlen((char const*)http_response), SOCKET_SEND_FLAG);
			}
			else if(strncmp(name,"/pb.js", strlen("/pb.js"))==0 )
			{
				memset(nstar_adt_web_tbuf,0,MAX_URI_SIZE);
				make_json_callback_cps(nstar_adt_web_tbuf);
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_adt_web_tbuf),nstar_adt_web_tbuf);
				send(s, (unsigned char *)http_response, strlen((char const*)http_response), SOCKET_SEND_FLAG);
			}
			else if(strncmp(name,"/sta.php", strlen("/sta.php"))==0 )
			{
				memset(nstar_adt_web_tbuf,0,MAX_URI_SIZE);
				make_json_callback_sta(nstar_adt_web_tbuf);
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_adt_web_tbuf),nstar_adt_web_tbuf);
				send(s, (unsigned char *)http_response, strlen((char const*)http_response), SOCKET_SEND_FLAG);
			}
			break;
		case METHOD_POST:												
			mid(http_request->URI, "/", " ", req_name);					
			if(strcmp(req_name,"config.cgi")==0)							  	
			{
				char jumpto[20];unsigned char maxlen=20;
				cgi_ipconfig(http_request, 1, jumpto, maxlen);			/*重启并保存*/
				make_cgi_response(5, (char*)config->local_ip, jumpto, nstar_adt_web_tbuf);	
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_adt_web_tbuf),nstar_adt_web_tbuf);																				/*发送http响应*/
				send(s, (unsigned char *)http_response, strlen((char *)http_response), SOCKET_SEND_FLAG);		
				//close(s);																					
				return;
			}
			else if(strcmp(req_name,"saveonly.cgi")==0)							  	
			{
				char jumpto[20];unsigned char maxlen=20;
				cgi_ipconfig(http_request, 0, jumpto, maxlen);			/*只保存*/
				make_cgi_noboot((char*)config->local_ip, jumpto, nstar_adt_web_tbuf);	
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_adt_web_tbuf),nstar_adt_web_tbuf);																				/*发送http响应*/
				send(s, (unsigned char *)http_response, strlen((char *)http_response), SOCKET_SEND_FLAG);		
				//close(s);																					
				return;
			}
			else if(strcmp(req_name,"bootmode.cgi")==0)							  	
			{
				char jumpto[20];unsigned char maxlen=20;
				cgi_ipconfig(http_request, 0, jumpto, maxlen);			/*只保存*/
				make_cgi_noboot((char*)config->local_ip, jumpto, nstar_adt_web_tbuf);	
				sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n%s",strlen(nstar_adt_web_tbuf),nstar_adt_web_tbuf);																				/*发送http响应*/
				send(s, (unsigned char *)http_response, strlen((char *)http_response), SOCKET_SEND_FLAG);		
				//close(s);	
				//iap_write_pack();
				//sys_cmm_restart();
				return;
			}
			break;
		default :
			break;
	}
}

void make_cgi_noboot(char* ip, char* jumptourl, char* cgi_response_buf)
{
  sprintf(cgi_response_buf,"<html><head><title>iWeb - Configuration</title><script language=javascript>;function func(){location.href='http://%d.%d.%d.%d%s';}</script></head><body onload='func()'></body></html>"
  	,ip[0],ip[1],ip[2],ip[3],jumptourl);
  return;
}



void cgi_ipconfig(st_http_request *http_request, unsigned char mode , char* jump_to, unsigned char jumplen)
{ 
	char *parm_url;
	//char *data; 
	//unsigned char	*buf = (unsigned char*)nstar_adt_web_rbuf;	//只有1个参数且很长，临时借用rxbuf;
	//unsigned int val;
	parm_url = (char*)get_param_url(http_request->URI, jump_to, jumplen);		/*获取修改后的IP地址*/
	if(parm_url == NULL){
		mydbg("parm url not found\r\n");
		return;
	}
	
//	data = get_http_param_value(buf, parm_url, "lip");		
//	if(data)
//	{	
//		syscfg_local_ip((char*)buf);
//	
//		
//	}

	//if(mode==1)
		//write_restart_cmd(5);
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


