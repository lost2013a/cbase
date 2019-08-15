#include "nstar_adt_httputil.h"
#include "nstar_adt_http_page.h"


#define JS_P1_E1 		"sip"
#define JS_P1_E2 		"dip"
#define JS_P1_E3 		"programIP"
#define JS_P1_E4 	    "logicID"
#define JS_P1_E5 	    "physID"

#define C_PAGE_NAME HTML_PAGE1_NAME

#define C_PAGE_BODY "<body>"\
"<ul>"\
"<li><a href='"HTML_PAGE1_NAME".html' class='active'>参数状态</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html'>网络设置</a></li>"\
"<li><a href='"HTML_PAGE3_NAME".html'>音频设置</a></li>"\
"<li><a href='"HTML_PAGE4_NAME".html'>信息日志</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3> 参数状态</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<form id='frmSetting' method='POST' action='"C_PAGE_NAME".cgi'>"\
"<p><label>服务器IP地址:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E1"' name='"JS_P1_E1"'/></p>"\
"<p><label>终端IP地址:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E2"' name='"JS_P1_E2"'/></p>"\
"<p><label>节目源地址:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E3"' name='"JS_P1_E3"'/></p>"\
"<p><label>逻辑地址:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E4"' name='"JS_P1_E4"'/></p>"\
"<p><label>物理地址:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E5"' name='"JS_P1_E5"'/></p>"\
"</form>"\
"</div>"\
"</body>"\
"</html>"


static void _add_htm_element(unsigned char mode)
{
	if(mode == URI_REPOS_HTML){
		REQUST_JSCRIPT_ELEMENT(JS_P1_E1);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E2);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E3);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E4);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E5);
	}
	else if(mode == URI_REPOS_JSON){
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E1, 192,1,1,170,40001 );
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E2, 192,1,1,1,51001);
		http_sprintf("'%s':'%d.%d.%d.%d',",JS_P1_E3, 255,255,255,0 );
		http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E4, 1,2,3,4,5,6 );
		http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E5, 1,2,3,4,5,6 );
	}
}

void parm1_pos_htm(unsigned char mode)
{
	if(mode == URI_REPOS_HTML)
		http_page_htm(C_PAGE_NAME, C_PAGE_BODY, _add_htm_element);
	else if(mode == URI_REPOS_JSON)
		http_page_json(C_PAGE_NAME, _add_htm_element);
}


static unsigned char set_sip(char* sip)
{
	printf("sip=%s\n", sip);	
	return 0;
}

void parm1_rpos_cgi(char *url)
{ 
	char *p_content; 
	p_content = (char*)my_get_param_url(url);
	if(NULL == p_content){
		return;
	}
	http_handle_parm(p_content, JS_P1_E1, (void*)set_sip);
	http_sprintf_init();
	http_sprintf(HTML_CGI_JUMP, LOCAL_IP, C_PAGE_NAME);
	http_sprintf_send();
}





