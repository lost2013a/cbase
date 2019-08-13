#include "htm_login.h"



#define COOKIE_HEAD1 "{ document.cookie='"
#define COOKIE_HEAD2 "'; window.location='home.html';}</script>"

#define HTML_VERIFY_PASS "<!DOCTYPE html><html>"\
"<head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'/></head>"\
"<body><span style='color:green;'>...验证通过 </span></body></html>"\
"<script>var int=setTimeout('refresh()',900);function refresh()"


#define HTML_VERIFY_FAILURE "<!DOCTYPE html><html>"\
"<head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'/></head>"\
"<body><span style='color:red;'>...验证失败 </span></body></html>"\
"<script>var int=setTimeout('refresh()',900);function refresh()"\
"{window.location='index.html';}</script>"


char tmpbuf[1000];

void cgi_log_in(st_http_request *http_request, unsigned char mode , char* jump_to, unsigned char jumplen)
{ 
	int idx=0;
	char *data; 
	char *parm_url;
	unsigned char *buf = (unsigned char*)nstar_web_rx_buf;	
	parm_url = (char*)get_param_url(http_request->URI, jump_to, jumplen);		/*获取修改后的IP地址*/
	if(parm_url == NULL){
		mydbg("parm url not found\r\n");
		return;
	}
	data = get_http_param_value(buf, parm_url, "login_pass");		
	if(data)
	{
		if(1 == login_pass_check(data)){
			sprintf(&tmpbuf[idx], "%s%s%s%s", HTML_VERIFY_PASS, COOKIE_HEAD1, cookie_getstr(), COOKIE_HEAD2);
			http_send((unsigned char *)tmpbuf, strlen((char const*)tmpbuf));
		}
		else
			http_send((unsigned char *)HTML_VERIFY_FAILURE, strlen((char const*)HTML_VERIFY_FAILURE));
	}
}




