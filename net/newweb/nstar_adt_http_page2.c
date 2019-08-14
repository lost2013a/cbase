#include <sys/socket.h>
#include <stdarg.h>
#include "nstar_adt_httputil.h"
#include "htm_login.h"

#include "nstar_adt_http_page.h"


#define JS_P1_E1 		"CmdPid"
#define JS_P1_E2 		"ProgPid"
#define JS_P1_E3 		"MP3Vol"
#define JS_P1_E4 	    "BitRate"
#define JS_P1_E5 	    "SamplRate"
#define JS_P1_E6 	    "Gain"


#define HTML_PAGE_BODY "<body>"\
"<ul>"\
"<li><a class='home'	href='home.html'>STAR-XINHUI</a></li>"\
"<li><a href='"HTML_PAGE1_NAME".html'>网络配置</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html' class='active'>参数设置</a></li>"\
"<li><a href='parm_3.html'>通道设置</a></li>"\
"<li><a href='parm_4.html'>信息日志</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3> 网络设置</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<form id='frmSetting' method='POST' action='"HTML_PAGE2_NAME".cgi'>"\
"<p><label>指令PID:</label><input type='text' size='16' id='"JS_P1_E1"' name='"JS_P1_E1"'/></p>"\
"<p><label>节目号:</label><input type='text' size='16' id='"JS_P1_E2"' name='"JS_P1_E2"'/></p>"\
"<p><label>MP3音量:</label><input type='text' size='16' id='"JS_P1_E3"' name='"JS_P1_E3"'/></p>"\
"<p><label>编码比特率:</label><input type='text' size='16' id='"JS_P1_E4"' name='"JS_P1_E4"'/></p>"\
"<p><label>编码采样率:</label><input type='text' size='16' id='"JS_P1_E5"' name='"JS_P1_E5"'/></p>"\
"<p><label>编码增益:</label><input type='text' size='16' id='"JS_P1_E6"' name='"JS_P1_E6"'/></p>"\
"<p><p><input type='button' value='保存' onclick ='commit();'  id='rcorners1'/>"\
"</form>"\
"</div>"\
"</body>"\
"</html>"

static void page_add_htm_element(unsigned char mode)
{
	if(mode == URI_REPOS_HTML){
		REQUST_JSCRIPT_ELEMENT(JS_P1_E1);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E2);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E3);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E4);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E5);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E6);
	}
	else if(mode == URI_REPOS_JSON){
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E1, 192,1,1,170,40001 );
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E2, 192,1,1,1,51001);
		http_sprintf("'%s':'%d.%d.%d.%d',",JS_P1_E3, 255,255,255,0 );
		http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E4, 1,2,3,4,5,6 );
		http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E5, 1,2,3,4,5,6 );
		http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E6, 1,2,3,4,5,6 );
	}
}

void parm2_pos_htm(unsigned char mode)
{
	if(mode == URI_REPOS_HTML)
		http_page_htm(HTML_PAGE2_NAME, HTML_PAGE_BODY, page_add_htm_element);
	else if(mode == URI_REPOS_JSON)
		http_page_json(HTML_PAGE2_NAME, page_add_htm_element);
}









