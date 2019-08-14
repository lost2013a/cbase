#include "nstar_adt_http_webpge.h"
#include "htm_login.h"

#define JSON_NAME_P1 	"JsonIP"
#define JSCRIPT_P1 		"ip.js"
#define JS_P1_E1 		"sip"
#define JS_P1_E2 		"dip"
#define JS_P1_E3 		"programIP"
#define JS_P1_E4 	    "logicID"
#define JS_P1_E5 	    "physID"

const char* html_body_parm1 "<body>"\
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
"<form id='frmSetting' method='POST'>"\
"<p><label>服务器IP地址:</label><input type='text' size='16' id=\""JS_P1_E1"\" name=\""JS_P1_E1"\"/></p>"\
"<p><label>终端IP地址:</label><input type='text' size='16' id=\""JS_P1_E2"\" name=\""JS_P1_E2"\"/></p>"\
"<p><label>节目源地址:</label><input type='text' size='16' id=\""JS_P1_E3"\" name=\""JS_P1_E3"\"/></p>"\
"<p><label>逻辑地址:</label><input type='text' size='16' id=\""JS_P1_E4"\" name=\""JS_P1_E4"\"/></p>"\
"<p><label>物理地址:</label><input type='text' size='16' disabled='disabled' id=\""JS_P1_E5"\" name=\""JS_P1_E5"\"/></p>"\
"<p><p><input type='button' value='保存' onclick ='saveonly();'  id='rcorners1'/>"\
"<input type='button' onclick ='savereboot();' value='保存并重启' id='rcorners1' style='background: #ff6300'/></p>"\
"<p style='color: #FF2222;font-size: 14px;'> 注意：参数修改后建议断电重启</p>"\
"</form>"\
"</div>"\
"</body>"\
"</html>"





















