#include "nstar_adt_httputil.h"
#include "nstar_adt_http_page.h"


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

static void _add_htm_element(unsigned char mode)
{

}

void parm5_pos_htm(unsigned char mode)
{
	if(mode == URI_REPOS_HTML)
		http_page_htm(C_PAGE_NAME, C_PAGE_BODY, _add_htm_element);
	else if(mode == URI_REPOS_JSON)
		http_page_json(C_PAGE_NAME, _add_htm_element);
}


#if 0	

#define TRUE 1
typedef struct __firmware{
	unsigned int fw_offset ;			//写FLASH地址
	unsigned int current_rxlen;			//当前接收长度
	unsigned int total_rxlen; 			//总的接收长度
	unsigned int total_w_len ;			//已写长度
	unsigned int hdr_len; 				//Content中，数据区的头部长度
	unsigned int data_len;				//Content中，数据区的长度
	unsigned int tail_len;				//Content中，数据区的尾部
	unsigned int real_wl; 				//真正写入FLASH的长度，需要4字节对齐
	unsigned char  remain_buf[3];		//未4字节对齐的数据保存位置
	unsigned char  remain_len ; 		//未4字节对齐的数据长度
	unsigned char  head_flag;			//找到数据起始位置的标志
	char *pos1;
	char *pos2;
	unsigned char *pdata;
}WEB_FIRMWARE;

unsigned char boundary[64];
unsigned char tmp_buf[1464]={0xff,};


#define verify_app_head(app_head) 1

#define WRITE_BUF[];

static int find_head(WEB_FIRMWARE *webfw, unsigned int content_len)
{
	if(webfw->head_flag != TRUE)
	{
		webfw->pos1= strstr((char*)webfw->pdata, (char*)boundary);		//在数据区中找头部分隔线。
		if(webfw->pos1 != NULL){
			//myprintf("找到分隔线\r\n");
			webfw->pos2= strstr(webfw->pos1+strlen((char*)boundary), "\r\n\r\n");
			if(webfw->pos2 == NULL){				//没找数据的起始区
				myprintf("没有找到升级文件数据区\r\n");
				return -1;
			}
			webfw->pos2+=4;
			webfw->pdata= (unsigned char*)webfw->pos2;
			webfw->head_flag = TRUE;
			//myprintf("找到升级文件数据区\r\n");
			webfw->hdr_len= (webfw->pos2-webfw->pos1)+2;	//多了1个"--"
			webfw->tail_len= strlen((int8*)boundary)+4+4; //多了2个"--"再加上2个"\r\n"
			webfw->data_len= content_len-webfw->hdr_len-webfw->tail_len;
			myprintf("数据长度:%ld= %d-%ld\r\n", webfw->data_len, content_len, webfw->hdr_len);
			bin_head *app_head= (bin_head*)(webfw->pdata);
			if(0 == verify_app_head(app_head)){
				myprintf("not match\n");
				return -1;	
			}
			if(webfw->total_rxlen == content_len){	//
				webfw->current_rxlen= webfw->data_len;
			}
			else{
				webfw->current_rxlen= webfw->total_rxlen-webfw->hdr_len;
			}	
			webfw->remain_len = webfw->current_rxlen % 4;
			memcpy(tmp_buf, webfw->pdata ,webfw->current_rxlen); //把当前收到数据的全拷贝
			webfw->real_wl= webfw->current_rxlen- webfw->remain_len;
			stm32_write_nocheck(webfw->fw_offset , (unsigned int*)tmp_buf, webfw->real_wl/4);
			webfw->fw_offset+=webfw->real_wl;
			webfw->total_w_len+=webfw->real_wl;
			if(webfw->remain_len!=0)
			{
				memcpy(webfw->remain_buf, &tmp_buf[webfw->real_wl] ,webfw->remain_len);
			}
			
		}	
//		else
//			myprintf("boundary null\r\n");
		return 1;
		
	}
	return 0;
}



static void rec_file(void)
			{
//Content 数据结构
//--boundary\r\n
//header\r\n\r\n
//data......
//\r\n--boundary--\r\n
//举例
/*
------WebKitFormBoundaryTnvuoPlJq5lIiOy7
Content-Disposition: form-data; name="fwfile"; filename="4.txt"
Content-Type: text/plain

1234
------WebKitFormBoundaryTnvuoPlJq5lIiOy7--
*/		
				WEB_FIRMWARE webfw={ADDR_FLASH_SECTOR_BAK,0};
				uint32 wcnt=0;						//异常计数
				uint32 content_len=0;		
				char jumpto[20];unsigned char maxlen=20;
				char sub[10];
				mydbg("raw:\r\n%s\r\n", &rx_buf[20]);
				mid((char*)http_request->URI,"Content-Length: ","\r\n",sub);		//找到文件长
				content_len=atoi(sub);
				mydbg("content_len=%ld \r\n", content_len);
				if(content_len > 1024*384){
					printf("固件长度不正常\r\n");
					disconnect(s);
					return;
				}
				mid((int8*)http_request->URI,"boundary=", "\r\n", (int8*)boundary);//找分割符
				mydbg("分割符boundary=%s\r\n",boundary);	
				webfw.pos1 = (char*)get_param_url(http_request->URI, jumpto, maxlen);
				if(webfw.pos1 == NULL){
					myerro("parm url not found\r\n");
					disconnect(s);
					return;
				}
				if(webfw.pos1- rx_buf < len-10){//目前收到的数据包含数据区
					webfw.pos1= strstr(&rx_buf[16], "\r\n\r\n");
					if(webfw.pos1 == NULL){
						myerro("content not found\r\n");
						disconnect(s);
						return;
					}
					webfw.pos1+=strlen("\r\n\r\n");
					webfw.pdata= (unsigned char*)webfw.pos1;
					webfw.current_rxlen=len- (webfw.pos1- rx_buf);
					webfw.total_rxlen+= webfw.current_rxlen;
					//myprintf("webfw.total_rxlen=%ld \r\n", webfw.total_rxlen);	
					if(find_head(&webfw, content_len) < 0){
						disconnect(s);
						return;
					}
				}
				else
					myprintf("1st too short\r\n");
				while(webfw.total_rxlen<content_len && wcnt<2000)
				{	
					webfw.current_rxlen=getSn_RX_RSR(s);
					if(webfw.current_rxlen>0){
						
						if(webfw.current_rxlen > 1460-webfw.remain_len) 
							webfw.current_rxlen= 1460-webfw.remain_len;
						//myarg(webfw.current_rxlen);
						webfw.current_rxlen= recv(s, (uint8*)rx_buf, webfw.current_rxlen);
						//myarg(webfw.current_rxlen);
						webfw.total_rxlen+= webfw.current_rxlen;
						//myarg(webfw.total_rxlen);
						webfw.pdata= (unsigned char*)rx_buf;
						int res= find_head(&webfw, content_len);
						if(res < 0){
							disconnect(s);
							return;
						}
						else if(res==1)
							continue;
						if(webfw.remain_len){	
							//myarg(webfw.remain_len);	//还存在上一次未对齐的数据
							memcpy(tmp_buf, webfw.remain_buf , webfw.remain_len);
						}
						memcpy(&tmp_buf[webfw.remain_len], rx_buf , webfw.current_rxlen);
						webfw.real_wl= webfw.current_rxlen+webfw.remain_len;
						
						if(webfw.total_rxlen >= content_len){
							webfw.real_wl-=webfw.tail_len;
							//myprintf("webfw.real_wl tail = %d\n", webfw.real_wl);
						}
						if(webfw.total_rxlen < content_len){
							webfw.remain_len = webfw.real_wl % 4;
							webfw.real_wl= webfw.real_wl-webfw.remain_len;
						}
						else{
							webfw.real_wl= ((webfw.real_wl+3)/4)*4;
						}
						stm32_write_nocheck(webfw.fw_offset , (unsigned int*)tmp_buf, webfw.real_wl/4);								
						#if(USE_WDOG == 1)
							IWDG_Feed();
						#endif
						webfw.fw_offset+=webfw.real_wl;
						if(webfw.remain_len!=0)
						{
							memcpy(webfw.remain_buf, &tmp_buf[webfw.real_wl] ,webfw.remain_len);
							//myprintf("webfw.remain_buf len:%d\r\n", webfw.remain_len);
						}
						//printf("content_len=%d webfw.total_rxlen=%d webfw.current_rxlen=%d\r\n", content_len, webfw.total_rxlen, webfw.current_rxlen); 			
						if(webfw.total_rxlen < content_len)	
							webfw.total_w_len+= webfw.current_rxlen;
						else
							webfw.total_w_len+= webfw.current_rxlen-webfw.tail_len;	//末尾时候，减去尾
						wcnt=0;	
						#if 0
						//W5300只做接收端，需要手动发空包告诉发送端，TCP滑动窗口的变化。针对某些情况，用HTML 空包 代替W5300的1字节空包
						const char vv[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:15\r\n\r\n{\"ver\":\"1\"}";
						send(s, (uint8_t *)vv, strlen(vv));
						#endif
					}
					else{
						wcnt++;
					}
				}
				if(webfw.total_rxlen == content_len){
					#if 0
					make_cgi_firmware_ret(10,(char*)config->local_ip, "/home.html", STR_NOTICE_OK, tx_buf);	
					sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s",strlen(tx_buf),tx_buf);																				/*发送http响应*/
					send(s, (uint8_t *)http_response, strlen((char *)http_response));	
					#endif
					disconnect(s);
					printf("升级文件接收完成\r\n");
					bin_head *app_head= (bin_head*)ADDR_FLASH_SECTOR_BAK;
					if(0 != verify_app_head(app_head))
						if(1 == verify_app_data((u8*)ADDR_FLASH_SECTOR_BAK)){
							sys_cmm_restart();
							return;
						}
						WEB_LOG_UPDATE_ERR2();
					}
				else{
					make_cgi_firmware_ret(3,(char*)config->local_ip, jumpto, STR_NOTICE_ERR, tx_buf);	
					sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s",strlen(tx_buf),tx_buf);																				/*发送http响应*/
					send(s, (uint8_t *)http_response, strlen((char *)http_response));	
					disconnect(s);
					printf("升级文件接收失败,len=%d/%d\r\n", webfw.total_rxlen,webfw.data_len);
					WEB_LOG_UPDATE_ERR3(webfw.total_rxlen, webfw.data_len);
				}
				return;	
			}

#endif


static unsigned char set_sip(char* sip)
{
	printf("sip=%s\n", sip);	
	return 0;
}

extern int test_http_rec(unsigned char *data, unsigned int rmax_len);


unsigned char boundary[64];
unsigned char tmp_buf[1464]={0xff,};


typedef struct __firmware{
	unsigned int fw_offset ;			//写FLASH地址
	unsigned int current_rxlen;			//当前接收长度
	unsigned int total_rxlen; 			//总的接收长度
	unsigned int total_w_len ;			//已写长度
	unsigned int hdr_len; 				//Content中，数据区的头部长度
	unsigned int data_len;				//Content中，数据区的长度
	unsigned int tail_len;				//Content中，数据区的尾部
	unsigned int real_wl; 				//真正写入FLASH的长度，需要4字节对齐
	unsigned char  remain_buf[3];		//未4字节对齐的数据保存位置
	unsigned char  remain_len ; 		//未4字节对齐的数据长度
	unsigned char  head_flag;			//找到数据起始位置的标志
	char *pos1;
	char *pos2;
	unsigned char *pdata;
}WEB_FIRMWARE;

WEB_FIRMWARE gwebfw;


static char* parse_head(const char* url)
{
	unsigned int wcnt=0;						//异常计数
	unsigned int content_len=0;		
	char *pstr, str_buf[20];
	pstr= http_mid((char*)url,"Content-Length: ","\r\n",str_buf);
	if(pstr == NULL)
		return NULL;
	content_len=atoi(str_buf);
	mydbg("content_len=%ld \r\n", content_len);
	if(content_len > 1024*384){
		printf("固件长度不正常\r\n");
		return NULL;
	}
	pstr= http_mid(pstr,"boundary=", "\r\n", (char*)boundary);//找分割符
	if(pstr == NULL)
		return NULL;
	mydbg("分割符boundary=%s\r\n",boundary); 
	return pstr;
	

}
			




static void find_date_boundary(const char* str)
{
		WEB_FIRMWARE *webfw= &gwebfw;
		webfw->pos1= strstr((char*)webfw->pdata, (char*)boundary);		//在数据区中找头部分隔线。
		if(webfw->pos1 != NULL){
			//myprintf("找到分隔线\r\n");
			webfw->pos2= strstr(webfw->pos1+strlen((char*)boundary), "\r\n\r\n");
			if(webfw->pos2 == NULL){				//没找数据的起始区
				myprintf("没有找到升级文件数据区\r\n");
				return ;
			}
			webfw->pos2+=4;
			webfw->pdata= (unsigned char*)webfw->pos2;
			webfw->head_flag = 1;
			//myprintf("找到升级文件数据区\r\n");
			webfw->hdr_len= (webfw->pos2-webfw->pos1)+2;	//多了1个"--"
			webfw->tail_len= strlen((char*)boundary)+4+4; //多了2个"--"再加上2个"\r\n"
			webfw->data_len= content_len-webfw->hdr_len-webfw->tail_len;
			myprintf("数据长度:%d= %d-%d\r\n", webfw->data_len, content_len, webfw->hdr_len);
		
			
		}	
		else
			myprintf("boundary null\r\n");


}

char tmpbuf[1024*1000];

void parm5_rpos_cgi(char *url)
{ 

//	const char vv[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:15\r\n\r\n{\"ver\":\"1\"}";
//	send(s, (uint8_t *)vv, strlen(vv));
	int len= test_http_rec((unsigned char*)tmpbuf, 1024*1000);
	
	printf("rec parm5_rpos_cgi:%d\n",len);
	parse_head(url);
	//find_head(len);
		return;
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







