#include "nstar_adt_httputil.h"
#include "nstar_adt_http_page.h"


#define JS_P1_E1 		"software"
#define JS_P1_E2 		"hadware"
#define JS_P1_E3 		"serial"
#define JS_P1_E4 		"logtxt"

#define C_PAGE_NAME HTML_PAGE4_NAME

#define C_PAGE_BODY \
"<body>"\
"<ul>"\
"<li><a href='"HTML_PAGE1_NAME".html'>参数状态</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html'>网络设置</a></li>"\
"<li><a href='"HTML_PAGE3_NAME".html'>音频设置</a></li>"\
"<li><a href='"HTML_PAGE4_NAME".html' class='active'>信息日志</a></li>"\
"<li><a href='"HTML_PAGE5_NAME".html'>面板固件</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3> 信息日志</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<form>"\
"<p><label>硬件版本:</label><input type='text' size='16' id='"JS_P1_E1"' name='"JS_P1_E1"'/></p>"\
"<p><label>软件版本:</label><input type='text' size='16' id='"JS_P1_E2"' name='"JS_P1_E2"'/></p>"\
"<p><label>序列号:</label><input type='text' size='16' id='"JS_P1_E3"' name='"JS_P1_E3"'/></p>"\
"<p><textarea style='color: #444444;font-size: 11px;' cols='140' rows='30' id='"JS_P1_E4"' name='"JS_P1_E4"'></textarea>"\
"</p></form>"\

char* read_log(void);
void sys_log(char* fmt,...);


static void _add_htm_element(unsigned char mode)
{
	if(mode == URI_REPOS_HTML){
		REQUST_JSCRIPT_ELEMENT(JS_P1_E1);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E2);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E3);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E4);
	}
	else if(mode == URI_REPOS_JSON){

#if 1
		{
			static unsigned int t=0;
				sys_log("只能怪大叔大婶多能怪大叔大婶多能怪大叔大婶多能怪大叔大婶多能怪大叔大婶多只能怪大叔大婶多能怪大叔大婶多能怪大叔大婶多能怪大叔大婶多能怪大叔大婶多只能怪大叔大婶多能怪大叔大婶多能怪大叔大婶多能怪大叔大婶多能怪大叔大婶多%03d", t++);
		}
#endif
	
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E1, 192,1,1,170,40001 );
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E2, 192,1,1,1,51001);
		http_sprintf("'%s':'%d.%d.%d.%d',",JS_P1_E3, 255,255,255,0 );
		http_sprintf("'%s':'%s',",JS_P1_E4, read_log() );
	}
}

void parm4_pos_htm(unsigned char mode)
{
	if(mode == URI_REPOS_HTML)
		http_page_htm(C_PAGE_NAME, C_PAGE_BODY, _add_htm_element);
	else if(mode == URI_REPOS_JSON)
		http_page_json(C_PAGE_NAME, _add_htm_element);
}



void parm4_rpos_cgi(char *url)
{ 
}


#define SYS_LOG_EN 1

#if (SYS_LOG_EN == 1) 
#include "stdio.h" 

#define LOG_STR_END "\\r\\n"	//web str end flag
#define LOG_MAX_LEN 4096
static char sys_logbuf[LOG_MAX_LEN+4]={0};
static unsigned short sys_log_len=0;

#ifndef NO_OS_LOCK
#define LOG_GET_LOCK()   
#define LOG_UNLOCK() 	
#else
static unsigned int h_loglock= 0;
#define LOG_GET_LOCK()   do{while(h_loglock!=0)delay_ms(10);h_loglock=1;}while(0)
#define LOG_UNLOCK()     do{h_loglock=0; }while(0)
#endif


static void write_log(const char *str)
{
		LOG_GET_LOCK();
		unsigned short rlen= LOG_MAX_LEN- sys_log_len;
		if(rlen > 128)
			sys_log_len+= snprintf(&sys_logbuf[sys_log_len], rlen, "%s%s", str, LOG_STR_END);
		else{	/*日志快满了了*/
			char* copy_line; 
			sys_logbuf[LOG_MAX_LEN]=0;
			/*移动3/4的旧日志到头部，以\r\n作为一条日志的标识*/
			copy_line= strstr(&sys_logbuf[sys_log_len/4], LOG_STR_END);
			if(copy_line!= NULL){
				copy_line+= strlen(LOG_STR_END);
				sys_log_len= &sys_logbuf[sys_log_len]- copy_line;
				memcpy(sys_logbuf, copy_line, sys_log_len);
			}
			else{
				sys_log_len=0;/*移动失败，清空*/
			}
			rlen= LOG_MAX_LEN- sys_log_len;
			sys_log_len+= snprintf(&sys_logbuf[sys_log_len], rlen, "%s%s", str, LOG_STR_END);
		}
		LOG_UNLOCK();
}


char* read_log(void)
{
	return sys_logbuf;
}

void sys_log(char* fmt,...)
{  
	#include <stdarg.h>
	#include "app_delay.h"
	int i=0;
	char log_temp[128]; 
	char* pbuf = log_temp;
	va_list ap;
	va_start(ap,fmt);
#if 1	
	unsigned int t= app_get_systime()/1000;
	i= sprintf((char*)pbuf,"%02d:%02d:%02d  ",(t/(60*60))%60, (t/60)%60, t%60);
#endif	
	//i=strlen((const char*)pbuf);		
	i= vsnprintf((char*)&pbuf[i], 127-i, fmt,ap);
	va_end(ap);
	write_log(pbuf);
}


#endif




