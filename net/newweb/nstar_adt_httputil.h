#ifndef	__HTTPUTIL_H__
#define	__HTTPUTIL_H__

#include "nstar_adt_http_server.h"
#include "ntart_cookie.h"

extern char *nstar_web_tx_buf; 
extern char *nstar_web_rx_buf;

#define	JSON_START_SYMBOL	"({"	
#define	JSON_END_SYMBOL		"});"

#define URI_REPOS_UNVAILD 0
#define URI_REPOS_HTML 1
#define URI_REPOS_JSON 2
#define URI_HTML ".html"
#define URI_JSON ".js"


#define HTML_PTYPE_HEAD "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n"
	
#define REQUST_JSCRIPT_HEAD "<script>"\
"function $(id) { return document.getElementById(id); };"\
"function json_%s(o) {"

#define REQUST_JSCRIPT_ELEMENT_STR		"if ($('%s')) $('%s').value = o.%s;"
#define REQUST_JSCRIPT_ELEMENT(element) http_sprintf(REQUST_JSCRIPT_ELEMENT_STR, element, element, element);


#define REQUST_JSCRIPT_END_STR "};function commit(){document.getElementById('frmSetting').submit();}"\
"function savereboot(){document.getElementById('frmSetting').action='"HTML_REBOOT_CGI_NAME".cgi'; document.getElementById('frmSetting').submit(); }</script>"\
"<script type='text/javascript' src='%s.js'> </script>"\


#define REQUST_JSCRIPT_END(name) 	http_sprintf(REQUST_JSCRIPT_END_STR, name);

#define HTML_PARM_HEAD "<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<title>斯达鑫辉网页管理界面</title>"\
"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>"\
"<style type='text/css'>"\
"label{display: inline-block;width: 120px;font-weight: bolder;color: #0066FF;}"\
"body {background-color: #F5F5F5;}"\
"h1 {font-size: 20px;padding: 8px 10%;font-weight: bolder;color: #FF0000;background-color: #FFFFFF;}"\
"h2 {text-align: center;font-size: 12px;font-style: oblique;font-weight: bold;color: #0066CC;}"\
"h3 {text-align: center;color: #333333;font-size: 24px;}"\
"ul {list-style-type: none;margin: 0px 10%; padding: 0;overflow: hidden;border: 1px solid #e7e7e7;"\
"background-color: #f3f3f3;}"\
"li { float: left;}"\
"li a {display: block;color: #666;text-align: center;padding: 10px 20px;text-decoration: none;}"\
"li a:hover:not(.active) {background-color: #CCCCCC;}"\
"li a.home {color: #F00;font-weight: bold;background-color: #FFFFFF;}"\
"li a.active {color: #F5F5F5;font-weight: bold;background-color: #0066CC;}"\
"#rcorners1{color: #FFFFFF;font-weight: bolder;font-size: 12px;border-radius: 15px;"\
"background: #0066CC;width: 110px;height: 30px;border: 0px;margin: 0.3rem 2rem 1rem 0rem;}"\
"</style>"\
"</head>"\

#define HTML_CGI_JUMP "<html><head><script language=javascript>;"\
"function func(){location.href='http://%s/%s.html';}</script></head><body onload='func()'></body></html>"


int do_https(void);

void http_sprintf_init(void);
void http_sprintf(char* fmt,...);
void http_sprintf_send(void);

void http_page_json(const char *name, void (fun_add_elemnet)(unsigned char));
void http_page_htm(const char *name, const char *body, void (fun_add_elemnet)(unsigned char));
void http_handle_parm(char *p_content, char *name, void(fun_set_parm)(char*));


char* web_file_frame1_content_len(unsigned int *content_len);

#endif


