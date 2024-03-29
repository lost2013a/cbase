#include <sys/socket.h>
#include <stdarg.h>
#include "nstar_adt_httputil.h"
#include "nstar_adt_http_webpge.h"
#include "htm_login.h"

#include "nstar_adt_http_page.h"


//const char *page1_name= "ipset";
//#define HTML_PAGE1_NAME "ipset"
#define JS_P1_E1 		"sip"
#define JS_P1_E2 		"dip"
#define JS_P1_E3 		"programIP"
#define JS_P1_E4 	    "logicID"
#define JS_P1_E5 	    "physID"


#define HTML_PAGE1_BODY "<body>"\
"<ul>"\
"<li><a class='home'	href='home.html'>STAR-XINHUI</a></li>"\
"<li><a class='active'	href='parm_1.html'>网络配置</a></li>"\
"<li><a href='parm_2.html'>参数设置</a></li>"\
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
"<p><p><input type='button' value='保存' onclick ='saveonly();'  id='rcorners1'/>"\
"<input type='button' onclick ='savereboot();' value='保存并重启' id='rcorners1' style='background: #ff6300'/></p>"\
"<p style='color: #FF2222;font-size: 14px;'> 注意：参数修改后建议断电重启</p>"\
"</form>"\
"</div>"\
"</body>"\
"</html>"


void parm1_pos_htm(void)
{
	const char *name= HTML_PAGE1_NAME;
	http_sprintf_init();
	http_sprintf("%s%s", HTML_PARM_HEAD, HTML_PAGE1_BODY);
	/*add script*/
	http_sprintf(REQUST_JSCRIPT_HEAD, name);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E1);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E2);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E3);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E4);
	REQUST_JSCRIPT_ELEMENT(JS_P1_E5);
	REQUST_JSCRIPT_END(HTML_PAGE1_NAME);
	
	http_sprintf_send();
}

void parm1_pos_json(void)
{
	const char *name= HTML_PAGE1_NAME;
	http_sprintf_init();
	http_sprintf("%s.json%s", name, JSON_START_SYMBOL);
	http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E1, 192,1,1,170,40001 );
	http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E2, 192,1,1,1,51001);
	http_sprintf("'%s':'%d.%d.%d.%d',",JS_P1_E3, 255,255,255,0 );
	http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E4, 1,2,3,4,5,6 );
	http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E5, 1,2,3,4,5,6 );
	http_sprintf(JSON_END_SYMBOL);
	http_sprintf_send();
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







