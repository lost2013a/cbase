#include "nstar_adt_httputil.h"
#include "nstar_adt_http_page.h"


#define JS_P1_E1 		"software"
#define JS_P1_E2 		"hadware"
#define JS_P1_E3 		"serial"
#define JS_P1_E4 		"logtxt"
#define JS_P1_E5 		"firmware"

#define C_PAGE_NAME HTML_PAGE4_NAME

#define C_PAGE_BODY \
"<style>"\
"progress{"\
"width: 468px;"\
"height: 5px;"\
"color:#ff6300;"\
"background:#E0E0E0;"\
"}"\
"progress::-webkit-progress-bar{     "\
"background-color: #E0E0E0;"\
"border-radius: 0.2rem;}"\
"progress::-webkit-progress-value{"\
"background: #ff6300;"\
"border-radius: 0.2rem; "\
"}"\
"</style>"\
"</head>"\
"<style>.txt{ height:28px;line-height:28px; border:1px solid #cdcdcd; width:180px;}"\
".ui-upload{"\
"height: 30px;width: 110px;background-color:#00abff;font-weight: bolder;font-size:12px;"\
"line-height:30px;display: inline-block;text-align:center;"\
"color: #ffffff;border-radius:15px;margin: 0px 30px 0px 30px;}</style>"\
"<body>"\
"<ul>"\
"<li><a href='"HTML_PAGE1_NAME".html'>参数状态</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html'>网络设置</a></li>"\
"<li><a href='"HTML_PAGE3_NAME".html'>音频设置</a></li>"\
"<li><a href='"HTML_PAGE4_NAME".html' class='active'>信息日志</a></li>"\
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
"<form id='frmFirmware' method='post' action='firmware.cgi' enctype='multipart/form-data'>"\
"<input type='text' id='textfield' class='txt'/>"\
"<label class='ui-upload'>选择升级文件<input type='file' id='file' name='myfile' value='' accept='.bin'style='display: none;'onchange=\"document.getElementById('textfield').value=this.files[0].name\"></label>"\
"<input type='button' onclick ='UpladFile();' value='开始升级' id='rcorners1' style='background: #ff6300'/>"\
"<br />"\
"<progress id='progressBar' value='0' max='100' ></progress>"\
"<span id='percentage'></span><span id='time' style='color: #FF2222;font-size: 14px;'></span>"\
"<p style='color: #FF2222;font-size: 14px;'> 注意：请在厂家技术指导下进行升级操作</a></p>"\
"</form>"\
"</div>"\
"</body>"\
"</html>"\
"</script>"\
"<script type='text/javascript'>"\
"var xhr;"\
"function UpladFile(){"\
"var fileObj = document.getElementById('file').files[0];"\
"var url = 'firmware.cgi';"\
"var form = new FormData();"\
"form.append('file', fileObj);"\
"xhr = new XMLHttpRequest();"\
"xhr.open('post', url, true);"\
"xhr.onload = uploadComplete;"\
"xhr.upload.onprogress = progressFunction;"\
"xhr.upload.onloadstart = function(){"\
"ot = new Date().getTime();"\
"oloaded = 0;};xhr.send(form);}"\
"function uploadComplete(evt) {"\
"var data = JSON.parse(evt.target.responseText);}"\
"function progressFunction(evt) {"\
"var progressBar = document.getElementById('progressBar');"\
"var percentageDiv = document.getElementById('percentage');"\
"if (evt.lengthComputable) {"\
"progressBar.max = evt.total;"\
"progressBar.value = evt.loaded;"\
"percentageDiv.innerHTML = Math.round(evt.loaded / evt.total * 100) + '%';}"\
"time.innerHTML = '，升级需要大约1分钟，请刷新页面，检查版本信息';"\
"}"\
"</script>"\



static void _add_htm_element(unsigned char mode)
{
	if(mode == URI_REPOS_HTML){
		REQUST_JSCRIPT_ELEMENT(JS_P1_E1);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E2);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E3);
		REQUST_JSCRIPT_ELEMENT(JS_P1_E4);
	}
	else if(mode == URI_REPOS_JSON){
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E1, 192,1,1,170,40001 );
		http_sprintf("'%s':'%d.%d.%d.%d:%d',",JS_P1_E2, 192,1,1,1,51001);
		http_sprintf("'%s':'%d.%d.%d.%d',",JS_P1_E3, 255,255,255,0 );
		http_sprintf("'%s':'%s',",JS_P1_E4, "1.test" );
	}
}

void parm4_pos_htm(unsigned char mode)
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

void parm4_rpos_cgi(char *url)
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







