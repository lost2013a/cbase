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
"</form>"\
"<form id='frmSetting' method='POST' action='"C_PAGE_NAME".cgi'>"\
"<p><label>烧写进度:</label><input type='text' size='16' disabled='disabled' id='"JS_P1_E1"' name='"JS_P1_E1"'/></p>"\
"</form>"\
"<p><p style='color: #FF2222;font-size: 14px;'> 注意：烧写过程中不要断电，烧写进度100%后刷新页面检查版本信息</a></p>"\
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
"progressBar.value = evt.loaded;}"\
"time.innerHTML = '固件上传进度'+Math.round(evt.loaded / evt.total * 100)+'%';"\
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



#define C_PAGE_BODY_NO_RIGHT \
"<body>"\
"<ul>"\
"<li><a href='"HTML_PAGE1_NAME".html'>参数状态</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html'>网络设置</a></li>"\
"<li><a href='"HTML_PAGE3_NAME".html'>音频设置</a></li>"\
"<li><a href='"HTML_PAGE4_NAME".html'>信息日志</a></li>"\
"<li><a href='"HTML_PAGE5_NAME".html' class='active'>固件升级</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3>面板固件升级</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<p style='color: #FF2222;font-size: 14px;'> 权限不足，请联系斯达鑫辉技术支持</a></p>"\
"</body>"\



extern int test_http_rec(unsigned char *data, unsigned int rmax_len);

typedef struct 
{
	unsigned int manufacture;
	unsigned int hard_ver;
	unsigned int soft_ver;
	unsigned int buildtime;
	unsigned int crc32val;
	unsigned int head_len;
	unsigned int data_len;
	unsigned int passwd;
	unsigned int align[120];
}bin_head;
#define BIN_HEAD_LEN (sizeof(bin_head))

#define MAX_FILE_LEN (3*1024*1024)
#define MIN_FILE_lEN (1024*1024)

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


static unsigned char verify_fw_head(void);
static unsigned char verify_fw_crc(void);


static unsigned char file_rate=0;
static void file_rate_add(void)
{
	if(file_rate < 90 && file_rate >= 10)
		file_rate+=10;
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
		http_sprintf("'%s':'%02d%%',",JS_P1_E1, read_file_rate() );
	}
	file_rate_add();
}

void http_html5_sta(const char* name, void (fun_add_elemnet)(unsigned char))
{
	http_sprintf_init(); 
#if 0	
	http_sprintf("%s%s", HTML_PARM_HEAD, C_PAGE_BODY_NO_RIGHT);
#else
	http_sprintf("%s%s", HTML_PARM_HEAD, C_PAGE_BODY);

	http_sprintf(STA_REFRESH_HTML_START);
	fun_add_elemnet(URI_REPOS_HTML);
	http_sprintf(STA_REFRESH_HTML_END);
#endif
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
	if(content_len > MAX_FILE_LEN || content_len < MIN_FILE_lEN){
		printf("err: Content-Length can't match\r\n");
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
	if(0 != parse_head(url)){
		printf("err: bad response, close now\r\n");
		return;
	}
	unsigned char pare_head=0;
	
	while(wcnt && p_webfile->current_rxlen < p_webfile->content_len){
		int len= test_http_rec((unsigned char*)nstar_web_rx_buf, MAX_URI_SIZE);
		if(len > 0){
			webfile_call_back((unsigned char*)nstar_web_rx_buf, len);
		}
		else{
			wcnt--;
			usleep(1*1000);
		}
		if(0 == pare_head && p_webfile->current_rxlen > MAX_URI_SIZE){
			pare_head= 1;
			file_handle(web_file_buf);
			if(1 != verify_fw_head()){
				wcnt=0;
				break;
			}
		}
		mydbg("file rec: %d/%d\n", p_webfile->current_rxlen, p_webfile->content_len);
	}
	if(wcnt!=0){
		if(1 == verify_fw_crc()){
			printf("file crc ok\n");
			file_rate_init();
		}
	}
	http_send((unsigned char *)ack, strlen(ack));
	return;
}


static unsigned int calculateCRC32 (const unsigned char *pStart, unsigned int uSize)
{
	const unsigned int CRC32_Table[256] =
	{
	   0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
	   0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	   0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	   0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	   0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
	   0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	   0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
	   0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	   0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	   0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	   0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
	   0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	   0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
	   0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	   0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	   0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	   0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
	   0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	   0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
	   0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	   0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	   0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	   0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
	   0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	   0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
	   0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	   0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	   0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	   0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
	   0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	   0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
	   0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	   0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
	   0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	   0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
	   0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	   0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
	   0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	   0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	   0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	   0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
	   0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	   0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
	   0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	   0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	   0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	   0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
	   0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	   0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
	   0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	   0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
	   0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	   0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
	   0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	   0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
	   0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	   0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	   0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	   0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
	   0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	   0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
	   0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	   0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	   0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
	};
	unsigned int uCRCValue;
	unsigned char *pData;

	uCRCValue = 0xFFFFFFFF;
	pData = (unsigned char*)pStart;
	while (uSize --)
	{
	uCRCValue = CRC32_Table[(uCRCValue ^ (*(pData++))) & 0xFF] ^ (uCRCValue >> 8);
	}
	return uCRCValue ^ 0xFFFFFFFF;
}


static unsigned char verify_fw_head(void)
{
const unsigned int manufacture= 0x1;
const unsigned int hard_ver	= 0x04000102;
const unsigned int soft_ver	= 0x1;
const unsigned int mypasswd	= 0x2919;

	WEB_FILE *p_webfile= &h_webfile;
	bin_head *bin= (bin_head*)p_webfile->file_data;
	unsigned char ret=1;
	char buf[40];
	if(bin->manufacture != manufacture){
		ret=0;
		snprintf(buf, 40, "厂商ID不符%x/%x", bin->manufacture, manufacture);
	}
	else if(bin->passwd != mypasswd){
		ret=0;
		snprintf(buf, 40, "文件类型不符%x/%x", bin->passwd, mypasswd);
	}
	else if(bin->hard_ver != hard_ver){
		ret=0;
		snprintf(buf, 40, "硬件版本不符%x/%x", bin->hard_ver, hard_ver);
	}	
	else if(bin->soft_ver <= soft_ver){
		ret=0;
		snprintf(buf, 40, "软件版本不符%x/%x", bin->soft_ver, soft_ver);
	}
	if(ret != 1){
		printf("err, verify file failed:%s\n", buf);
		sys_log("web升级失败:%s", buf);
	}
	return ret;
}



static unsigned char verify_fw_crc(void)
{
	WEB_FILE *p_webfile= &h_webfile;
	bin_head *bin= (bin_head*)p_webfile->file_data;
	unsigned int file_len= p_webfile->file_len;
	unsigned int rec_crc= bin->crc32val, cal_crc;
	unsigned char ret=1;
	char buf[40];
	printf("len= %08x / %08x \n", bin->data_len + BIN_HEAD_LEN, file_len);
	
	if(bin->data_len + BIN_HEAD_LEN !=  file_len){
		snprintf(buf, 40, "文件长度不符%d/%d", file_len, bin->data_len + BIN_HEAD_LEN);
		ret=0;
	}
	else{	
		#if 1
		unsigned char *p_crc_start = (unsigned char*)bin;
		bin->crc32val= 0;
		cal_crc=calculateCRC32(p_crc_start, file_len);
		#else
		unsigned char *p_crc_start = (unsigned char*)bin;
		p_crc_start+= BIN_HEAD_LEN;
		cal_crc=calculateCRC32(p_crc_start, file_len- BIN_HEAD_LEN);
		#endif
		
		printf("crc= %08x / %08x \n", rec_crc, cal_crc);

		if(rec_crc !=  cal_crc){
			snprintf(buf, 40, "文件校验错误%x/%x", cal_crc, rec_crc);
			ret=0;
		}
	}
	if(ret != 1){
		printf("err, verify file failed:%s\n", buf);
		sys_log("web升级失败:%s", buf);
	}
	return ret;
}



