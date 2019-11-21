#include <sys/socket.h>
#include <stdarg.h>
#include "nstar_adt_httputil.h"
#include "nstar_adt_http_page.h"



#define C_PAGE_NAME  HTML_PAGE0_NAME
#define JUMP_TO_HTML HTML_PAGE1_NAME

#define USER_NAME  "usname"
#define PASSW_NAME "passw"
#define BOARD_NAME "适配器"

#define HTML_LOG_IN "<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<title>斯达鑫辉终端网页管理</title>"\
"<meta http-equiv='Content-Type' content='text/html' charset='utf-8'/>"\
"<style type='text/css'>"\
"label{display: inline-block;width: 80px;font-weight: bolder;color: #0066FF;}"\
"body {"\
"	background-color: #35a6e8;"\
"	background: -webkit-linear-gradient(right, #f0f0f0 , #35a6e8);"\
"    background: -o-linear-gradient( right, #f0f0f0, #35a6e8);"\
"    background: -moz-linear-gradient( right, #f0f0f0, #35a6e8);"\
"    background: linear-gradient(to right, #f0f0f0 , #35a6e8);"\
"}"\
"h1 {font-size: 20px;padding: 8px 10%;font-weight: bolder;color: #FF0000;background-color: #FFFFFF;}"\
"h2 {text-align: center;font-size: 12px;font-style: oblique;font-weight: bold;color: #0066CC;}"\
"h3 {text-align: left;color: #ffffff;font-size: 24px;font-weight: bolder;}"\
"h4 {color: #78e4ca;font-size: 33px;font-style: oblique;}"\
"h5 {text-align: end;color: #FFFFFF;font-size: 11px;}"\
"h6 {text-align: left;font-size: 12px;font-style: oblique;font-weight: bold;color: #0066CC;}"\
".wrapper {width:720px;margin:auto;}"\
".wrapper1 {"\
"	width: 310px;"\
"	height: 360px;"\
"	padding: 20px;"\
"	background: #5e85c0;"\
"	float: left;"\
"	border: 5px;"\
"	border-style:solid;"\
"	border-color:#ffffff;"\
"	background: -webkit-linear-gradient(left top, #78e4ca , #506de3);"\
"	background: -o-linear-gradient(bottom right, #78e4ca, #506de3);"\
"	background: -moz-linear-gradient(bottom right, #78e4ca, #506de3);"\
"	background: linear-gradient(to bottom right, #78e4ca , #506de3);"\
"}"\
".wrapper2 {"\
"	width: 330px;"\
"	height: 380px;"\
"	padding: 10px;"\
"	background: #f0f0f0;"\
"	float: left;"\
"	border: 5px;"\
"	border-style:solid;"\
"	border-color:#ffffff;"\
"}"\
"#rcorners1 {"\
"	color: #FFFFFF;"\
"	font-weight: bolder;"\
"	font-size: 18px;"\
"	border-radius: 15px;"\
"	background: #4cbdf7;"\
"	background: #0066CC;"\
"	width: 158px;"\
"	height: 30px;"\
"	border: 0px;"\
"}"\
"</style>"\
"</head>"\
"<body>"\
"<h1 > STAR-XINHUI </h1>"\
"<h2> chengdu sida web management interface </h2>"\
"<br><br><br><br><br><br>"\
"<div class='wrapper' >"\
"<div class='wrapper1' > "\
"<h4> Welcome！ </h4>"\
"<h3> 终端网页管理界面Ver1.1 </h3>"\
"<br><br><br><br>"\
"<h5> 型号："BOARD_NAME"<br>&copy;斯达鑫辉视讯科技有限公司 </h5>"\
"</div>"\
"<div class='wrapper2' >"\
"<div style='padding:100px 00px 00px 85px;'>"\
"<h6> 用户密码：</h6>	"\
"</div>"\
"<form id='frmSetting' method='POST' action='"C_PAGE_NAME".cgi'>"\
"<div style='padding:0px 00px 00px 0px;'>"\
"<p>"\
"<label for='txtpass' ></label> "\
"<input type='text' value='请输入用户密码'  size='16' style='font-size: 16px; color: #c0c0c0' name='"PASSW_NAME"'"\
"onFocus=\"if(value==defaultValue){value='';this.style.color='#303030'}\""\
"onBlur=\"if(!value){value=defaultValue;this.style.color='#c0c0c0'}\" />"\
"</p>"\
"</div>"\
"<div style='padding:1px 00px 00px 85px;'>"\
"<p> <input  type='submit' value='登录' id='rcorners1'/></p>"\
"</form>"\
"</div>"\
"</div>"\
"</div>"

#define HTML_VERIFY_PASS "<!DOCTYPE html><html>"\
"<head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'/></head>"\
"<body><span style='color:green;'>...验证通过</span></body></html>"\
"<script>var int=setTimeout('refresh()',450);function refresh(){"\
"document.cookie='%s'; window.location='"JUMP_TO_HTML".html';}</script>"

#define HTML_VERIFY_FAILURE "<!DOCTYPE html><html>"\
"<head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'/></head>"\
"<body><span style='color:red;'>...验证失败</span></body></html>"\
"<script>var int=setTimeout('refresh()',900);function refresh(){"\
"window.location='"C_PAGE_NAME".html';}</script>"


#define HTML_VERIFY_TIMEOUT "<!DOCTYPE html><html>"\
"<head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'/></head>"\
"<body><span style='color:red;'>...验证过期</span></body></html>"\
"<script>var int=setTimeout('refresh()',900);function refresh(){"\
"window.location='"C_PAGE_NAME".html';}</script>"


void parm0_pos_htm(unsigned char mode)
{
	http_sprintf_init();
	http_sprintf(HTML_LOG_IN);
	http_sprintf_send();
}



unsigned char login_pass_check(const char* dst)
{
	
#define USER_PASS  "admin"
#define SUPPER_PASS "newstar2919"
#define PASS_LEN (strlen(USER_PASS))
#define SUPPER_PASS_LEN (strlen(SUPPER_PASS))

	unsigned char ret=0;
	if(dst == NULL)
		return ret;
	if(memcmp(SUPPER_PASS, dst, SUPPER_PASS_LEN) == 0)
		ret=1;
	else if(memcmp(USER_PASS, dst, PASS_LEN) == 0)
		ret=1;
	return ret;
}


void parm0_rpos_cgi(char *url)
{ 
	char *p_content; 
	p_content = (char*)my_get_param_url(url);
	if(NULL == p_content){
		return;
	}
	char *p_parm= get_http_param_value((unsigned char*)nstar_web_rx_buf, p_content, PASSW_NAME);		
	if(p_parm)
	{
		http_sprintf_init();
		if(1 == login_pass_check(p_parm)){
			http_sprintf(HTML_VERIFY_PASS, cookie_getstr());
		}
		else{	
			http_sprintf(HTML_VERIFY_FAILURE);	
		}
		http_sprintf_send();
	}
}


void verify_time_out(void)
{ 
	http_sprintf_init();
	http_sprintf(HTML_VERIFY_TIMEOUT);	
	http_sprintf_send();
}





