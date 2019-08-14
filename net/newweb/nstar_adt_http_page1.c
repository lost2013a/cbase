#include <sys/socket.h>
#include <stdarg.h>
#include "nstar_adt_httputil.h"
#include "htm_login.h"
#include "nstar_adt_http_page.h"


#define JS_P1_E1 		"sip"
#define JS_P1_E2 		"dip"
#define JS_P1_E3 		"programIP"
#define JS_P1_E4 	    "logicID"
#define JS_P1_E5 	    "physID"

#define HTML_PAGE1_BODY "<body>"\
"<ul>"\
"<li><a class='home'	href='home.html'>STAR-XINHUI</a></li>"\
"<li><a href='"HTML_PAGE1_NAME".html' class='active'>网络配置</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html'>参数设置</a></li>"\
"<li><a href='parm_3.html'>通道设置</a></li>"\
"<li><a href='parm_4.html'>信息日志</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3> 网络设置</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<form id='frmSetting' method='POST' action='"HTML_PAGE1_NAME".cgi'>"\
"<p><label>服务器IP地址:</label><input type='text' size='16' id='"JS_P1_E1"' name='"JS_P1_E1"'/></p>"\
"<p><label>终端IP地址:</label><input type='text' size='16' id='"JS_P1_E2"' name='"JS_P1_E2"'/></p>"\
"<p><label>节目源地址:</label><input type='text' size='16' id='"JS_P1_E3"' name='"JS_P1_E3"'/></p>"\
"<p><label>逻辑地址:</label><input type='text' size='16' id='"JS_P1_E4"' name='"JS_P1_E4"'/></p>"\
"<p><label>物理地址:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E5"' name='"JS_P1_E5"'/></p>"\
"<p><p><input type='button' value='保存' onclick ='commit();'  id='rcorners1'/>"\
"</form>"\
"</div>"\
"</body>"\
"</html>"

static void page1_add_htm_element(unsigned char mode)
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
		http_page_htm(HTML_PAGE1_NAME, HTML_PAGE1_BODY, page1_add_htm_element);
	else if(mode == URI_REPOS_JSON)
		http_page_json(HTML_PAGE1_NAME, page1_add_htm_element);
}


void parm1_rpos_cgi(st_http_request *http_request, unsigned char mode , char* jump_to, unsigned char jumplen)
{ 
	char *parm_url;
	char *data; 
	unsigned char *buf = (unsigned char*)nstar_web_rx_buf;	//只有1个参数且很长，临时借用rxbuf;
	unsigned int val;
	parm_url = (char*)get_param_url(http_request->URI, jump_to, jumplen);
	if(parm_url == NULL){
		mydbg("parm url not found\r\n");
		return;
	}
	data = get_http_param_value(buf, parm_url, JS_P1_E1);		
	if(data)
	{	
		printf("JS_P1_E1=%s\n", data);	
	}
}





