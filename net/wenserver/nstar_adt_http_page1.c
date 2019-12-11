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
"<li><a class='home'>STAR-斯达鑫辉</a></li>"\
"<li><a href='"HTML_PAGE1_NAME".html' class='active'>参数状态</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html'>网络设置</a></li>"\
"<li><a href='"HTML_PAGE3_NAME".html'>音频设置</a></li>"\
"<li><a href='"HTML_PAGE4_NAME".html'>信息日志</a></li>"\
"<li><a href='"HTML_PAGE5_NAME".html'>面板固件</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3> 参数状态</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<form id='frmSetting' method='POST' action='"C_PAGE_NAME".cgi'>"\
"<p><label>工作模式:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E1"' name='"JS_P1_E1"'/></p>"\
"<p><label>指令端口状态:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E2"' name='"JS_P1_E2"'/></p>"\
"<p><label>编码器状态:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E3"' name='"JS_P1_E3"'/></p>"\
"<p><label>数据流量:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E4"' name='"JS_P1_E4"'/></p>"\
"<p><label>当前音源:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E5"' name='"JS_P1_E5"'/></p>"\
"</form>"\
"</div>"\
"</body>"\
"</html>"\

#define STA_REFRESH_HTML_START "<script>function $(id) { return document.getElementById(id); };"\
"function myrefresh()"\
"{"\
"	var xmlHttp = new XMLHttpRequest();"\
"	xmlHttp.open('GET', '/sta.js', true);"\
"	xmlHttp.send(null);"\
"	xmlHttp.onreadystatechange=function()"\
"	{"\
"		if(xmlHttp.readyState == 4 && xmlHttp.status == 200) "\
"		{"\
"var json=xmlHttp.responseText;"\
"var o=eval(\'(\'+json+\')\');"\

#define STA_REFRESH_HTML_END  "}"\
"	}"\
"}</script>	"\
"<script language='JavaScript'>setInterval('myrefresh()',2000);</script>"\
"<script>myrefresh();</script>"




static void _add_htm_element(unsigned char mode)
{
	static int i=0;
	if(mode == URI_REPOS_HTML){
		REQUST_JSCRIPT_ELEMENT(JS_P1_E1);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E2);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E3);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E4);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E5);
	}
	else if(mode == URI_REPOS_JSON){
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E1, 192,1,1,170,i++ );
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E2, 192,1,1,1,51001);
		http_sprintf("'%s':'%d.%d.%d.%d',",JS_P1_E3, 255,255,255,0 );
		http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E4, 1,2,3,4,5,6 );
		http_sprintf("'%s':'%02X-%02X-%02X-%02X-%02X-%02X',",JS_P1_E5, 1,2,3,4,5,6 );
	}
}

void http_html_sta(const char* name, void (fun_add_elemnet)(unsigned char))
{
	http_sprintf_init();
	http_sprintf("%s%s", HTML_PARM_HEAD, C_PAGE_BODY);
	http_sprintf(STA_REFRESH_HTML_START);
	fun_add_elemnet(URI_REPOS_HTML);
	http_sprintf(STA_REFRESH_HTML_END);
	http_sprintf_send();
}


void http_json_sta(void (fun_add_elemnet)(unsigned char))
{

	http_sprintf_init();
	http_sprintf("{");
	fun_add_elemnet(URI_REPOS_JSON);
	http_sprintf("}");
	http_sprintf_send();
}



void parm1_pos_htm(unsigned char mode)
{
	if(mode == URI_REPOS_HTML){
		http_html_sta(HTML_PAGE1_NAME, _add_htm_element);
	}
	else{
	
		http_json_sta(_add_htm_element);
	
	}
}






