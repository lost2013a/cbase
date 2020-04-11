#include "nstar_adt_httputil.h"
#include "nstar_adt_http_page.h"
#include "unistd.h"


#define UPDATE_FILE 	"firmware"

#define C_PAGE_NAME HTML_PAGE5_NAME
#define mydbg printf
#define myprintf printf

#define JS_P1_E1 		"CmdPid"
#define C_PAGE_BODY "<style>"\
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
"</div>"\
"<body>"\
"<ul>"\
"<li><a href='"HTML_PAGE1_NAME".html'>参数状态</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html'>网络设置</a></li>"\
"<li><a href='"HTML_PAGE3_NAME".html'>音频设置</a></li>"\
"<li><a href='"HTML_PAGE4_NAME".html'>信息日志</a></li>"\
"<li><a href='"HTML_PAGE5_NAME".html' class='active'>面板固件</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3>面板固件升级</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<form id='frmFirmware' method='post' action='firmware.cgi' enctype='multipart/form-data'>"\
"<input type='text' id='textfield' class='txt'/>"\
"<label class='ui-upload'>选择升级文件<input type='file' id='file' name='myfile' value='' accept='.bin'style='display: none;'onchange=\"document.getElementById('textfield').value=this.files[0].name\"></label>"\
"<input type='button' onclick ='UpladFile();' value='开始升级' id='rcorners1' style='background: #ff6300'/>"\
"<br />"\
"<progress id='progressBar' value='0' max='100' ></progress>"\
"<span id='percentage'></span><span id='time' style='color: #FF2222;font-size: 14px;'></span>"\
"<p style='color: #FF2222;font-size: 14px;'> 注意：用于面板程序升级，请在厂家技术指导下进行升级操作</a></p>"\
"</form>"\
"<form id='frmSetting' method='POST' action='"C_PAGE_NAME".cgi'>"\
"<p><label>烧写进度:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E1"' name='"JS_P1_E1"'/></p>"\
"</form>"\
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
"time.innerHTML = '，升级过程需要大约1分钟，升级后请检查版本信息';"\
"}"\
"</script>"\

#define STA_REFRESH_HTML_START "<script>function $(id) { return document.getElementById(id); };"\
"function myrefresh()"\
"{"\
"	var xmlHttp = new XMLHttpRequest();"\
"	xmlHttp.open('GET', '/firmware.js', true);"\
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
"<script language='JavaScript'>setInterval('myrefresh()',1000);</script>"\
"<script>myrefresh();</script>"



extern int test_http_rec(unsigned char *data, unsigned int rmax_len);

#define MAX_FILE_LEN (1024*1024)
unsigned char tmp_buf[1464]={0xff,};
unsigned char web_file_buf[MAX_FILE_LEN];

typedef struct __webfile{
	/*把file的中间量做出外部变量，便于file过程中，做其他事情，比如写入FLASH*/
	unsigned char *file_data;			//file数据地址
	unsigned int file_len; 				//file数据长度
	unsigned int content_len ;			//html 内容长度
	unsigned int current_rxlen;			//html 当前接收长度
	unsigned int total_rxlen; 			//html 总的接收长度
	char boundary[64];					//html web file 分割符
}WEB_FILE;

WEB_FILE h_webfile;

static unsigned char file_rate=0;
static void file_rate_add(void)
{
	if(file_rate < 100 && file_rate >= 10)
		file_rate+=20;
}
static void file_rate_init(void)
{
	file_rate=10;
}
static void file_rate_100(void)
{
	file_rate=100;
}
static unsigned char read_file_rate(void)
{
	return file_rate;
}


static void _add_htm_element(unsigned char mode)
{
	if(mode == URI_REPOS_HTML){
		REQUST_JSCRIPT_ELEMENT(JS_P1_E1);
		
	}
	else if(mode == URI_REPOS_JSON){
		http_sprintf("'%s':'%02d\%',",JS_P1_E1, read_file_rate() );
	}
	file_rate_add();
}

void http_html5_sta(const char* name, void (fun_add_elemnet)(unsigned char))
{
	http_sprintf_init();
	http_sprintf("%s%s", HTML_PARM_HEAD, C_PAGE_BODY);
	http_sprintf(STA_REFRESH_HTML_START);
	fun_add_elemnet(URI_REPOS_HTML);
	http_sprintf(STA_REFRESH_HTML_END);
	http_sprintf_send();
}

void http_json5_sta(void (fun_add_elemnet)(unsigned char))
{

	http_sprintf_init();
	http_sprintf("{");
	fun_add_elemnet(URI_REPOS_JSON);
	http_sprintf("}");
	http_sprintf_send();
}



void parm5_pos_htm(unsigned char mode)
{
	if(mode == URI_REPOS_HTML){
		http_html5_sta(HTML_PAGE5_NAME, _add_htm_element);
	}
	else{
	
		http_json5_sta(_add_htm_element);
	
	}
	
}



//void parm5_pos_htm(unsigned char mode)
//{
//	if(mode == URI_REPOS_HTML)
//		http_page_htm(C_PAGE_NAME, C_PAGE_BODY, _add_htm_element);
//	else if(mode == URI_REPOS_JSON)
//		http_page_json(C_PAGE_NAME, _add_htm_element);
//}

static unsigned char webfile_call_back(unsigned char *data, unsigned int datalen)
{
	WEB_FILE *p_webfile= &h_webfile;
	unsigned char *idx= web_file_buf+ p_webfile->current_rxlen;
	memcpy(idx, data, datalen);
	p_webfile->current_rxlen+= datalen;
	return 1;
}

static int _file_head_handle(void)
{
	WEB_FILE *p_webfile= &h_webfile;
	unsigned int len;
	char *data;
	data= web_file_frame1_content_len(&len);
	if(len == 0){
		printf("err: can't get file info\n");
		return -4;
	}
	p_webfile->current_rxlen= 0;
	webfile_call_back((unsigned char*)data, len);
	return 0;
}

static int parse_head(const char* url)
{
	unsigned int content_len=0;		
	char *pstr, *p_bound, str_buf[20];
	WEB_FILE *p_webfile= &h_webfile;
	pstr= http_mid((char*)url,"Content-Length: ","\r\n",str_buf);
	if(pstr == NULL){
		printf("err: Content-Length can't find\r\n");
		return -1;
	}
	content_len=atoi(str_buf);
	if(content_len > MAX_FILE_LEN){
		printf("err: Content-Length too long\r\n");
		return -2;
	}
	p_webfile->content_len= content_len;
	p_bound= p_webfile->boundary;
	pstr= http_mid((char*)url,"boundary=", "\r\n", p_bound);//找分割符
	if(pstr == NULL){
		printf("err: boundary can't find\r\n"); 
		return -3;
	}
	return _file_head_handle();
}

static int file_handle(unsigned char *data)
{
	WEB_FILE *p_webfile= &h_webfile;
	unsigned int hdr_len, tail_len;
	char *pos1,*pos2;

	pos1= strstr((char*)data, (char*)p_webfile->boundary);
	if(pos1 == NULL){
		printf("err: boundary can't find\r\n"); 
		return -1;
	}
	pos2= strstr(pos1+strlen((char*)p_webfile->boundary), "\r\n\r\n");
	if(pos2 == NULL){
		printf("err: file head can't find\r\n");
		return -1;
	}
	pos2+=4;
	p_webfile->file_data= (unsigned char*)pos2;
	hdr_len= (pos2- pos1)+2;	//多了1个"--"
	tail_len= strlen((char*)p_webfile->boundary)+4+4; //多了2个"--"再加上2个"\r\n"
	p_webfile->file_len= p_webfile->content_len-hdr_len-tail_len;
	
	myprintf("file len:%u= %u-%u-%u\r\n", p_webfile->file_len, p_webfile->content_len, hdr_len, tail_len);
	return 0;
}



void parm5_rpos_cgi(char *url)
{ 
	extern int http_send(unsigned char *data, unsigned int len);
#define TIME_OUT_CNT 100
	int wcnt= TIME_OUT_CNT;
	WEB_FILE *p_webfile= &h_webfile;
	const char ack[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:15\r\n\r\n{\"ver\":\"1\"}";
	file_rate_init();
	if(0 != parse_head(url)){
		printf("err: bad response, close now\r\n");
		return;
	}
	
	while(wcnt && p_webfile->current_rxlen < p_webfile->content_len){
		int len= test_http_rec((unsigned char*)nstar_web_rx_buf, MAX_URI_SIZE);
		if(len > 0){
			webfile_call_back((unsigned char*)nstar_web_rx_buf, len);
		}
		else{
			wcnt--;
		}
		mydbg("file rec: %d/%d\n", p_webfile->current_rxlen, p_webfile->content_len);
		usleep(10*1000);
	}
	if(wcnt!=0){
		file_handle(web_file_buf);
	}
	http_send((unsigned char *)ack, strlen(ack));
	return;
}







