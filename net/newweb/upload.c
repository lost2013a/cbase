#include "nstar_adt_httputil.h"
#include "nstar_adt_http_page.h"


#define UPDATE_FILE 	"firmware"

#define C_PAGE_NAME HTML_PAGE5_NAME

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
"<li><a href='"HTML_PAGE1_NAME".html'>����״̬</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html'>��������</a></li>"\
"<li><a href='"HTML_PAGE3_NAME".html'>��Ƶ����</a></li>"\
"<li><a href='"HTML_PAGE4_NAME".html'>��Ϣ��־</a></li>"\
"<li><a href='"HTML_PAGE5_NAME".html' class='active'>���̼�</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3>���̼�����</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<form id='frmFirmware' method='post' action='firmware.cgi' enctype='multipart/form-data'>"\
"<input type='text' id='textfield' class='txt'/>"\
"<label class='ui-upload'>ѡ�������ļ�<input type='file' id='file' name='myfile' value='' accept='.bin'style='display: none;'onchange=\"document.getElementById('textfield').value=this.files[0].name\"></label>"\
"<input type='button' onclick ='UpladFile();' value='��ʼ����' id='rcorners1' style='background: #ff6300'/>"\
"<br />"\
"<progress id='progressBar' value='0' max='100' ></progress>"\
"<span id='percentage'></span><span id='time' style='color: #FF2222;font-size: 14px;'></span>"\
"<p style='color: #FF2222;font-size: 14px;'> ע�⣺�������������������ڳ��Ҽ���ָ���½�����������</a></p>"\
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
"time.innerHTML = '������������Ҫ��Լ1���ӣ�����������汾��Ϣ';"\
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
	unsigned int fw_offset ;			//дFLASH��ַ
	unsigned int current_rxlen;			//��ǰ���ճ���
	unsigned int total_rxlen; 			//�ܵĽ��ճ���
	unsigned int total_w_len ;			//��д����
	unsigned int hdr_len; 				//Content�У���������ͷ������
	unsigned int data_len;				//Content�У��������ĳ���
	unsigned int tail_len;				//Content�У���������β��
	unsigned int real_wl; 				//����д��FLASH�ĳ��ȣ���Ҫ4�ֽڶ���
	unsigned char  remain_buf[3];		//δ4�ֽڶ�������ݱ���λ��
	unsigned char  remain_len ; 		//δ4�ֽڶ�������ݳ���
	unsigned char  head_flag;			//�ҵ�������ʼλ�õı�־
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
		webfw->pos1= strstr((char*)webfw->pdata, (char*)boundary);		//������������ͷ���ָ��ߡ�
		if(webfw->pos1 != NULL){
			//myprintf("�ҵ��ָ���\r\n");
			webfw->pos2= strstr(webfw->pos1+strlen((char*)boundary), "\r\n\r\n");
			if(webfw->pos2 == NULL){				//û�����ݵ���ʼ��
				myprintf("û���ҵ������ļ�������\r\n");
				return -1;
			}
			webfw->pos2+=4;
			webfw->pdata= (unsigned char*)webfw->pos2;
			webfw->head_flag = TRUE;
			//myprintf("�ҵ������ļ�������\r\n");
			webfw->hdr_len= (webfw->pos2-webfw->pos1)+2;	//����1��"--"
			webfw->tail_len= strlen((int8*)boundary)+4+4; //����2��"--"�ټ���2��"\r\n"
			webfw->data_len= content_len-webfw->hdr_len-webfw->tail_len;
			myprintf("���ݳ���:%ld= %d-%ld\r\n", webfw->data_len, content_len, webfw->hdr_len);
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
			memcpy(tmp_buf, webfw->pdata ,webfw->current_rxlen); //�ѵ�ǰ�յ����ݵ�ȫ����
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




			{
//Content ���ݽṹ
//--boundary\r\n
//header\r\n\r\n
//data......
//\r\n--boundary--\r\n
//����
/*
------WebKitFormBoundaryTnvuoPlJq5lIiOy7
Content-Disposition: form-data; name="fwfile"; filename="4.txt"
Content-Type: text/plain

1234
------WebKitFormBoundaryTnvuoPlJq5lIiOy7--
*/		
				WEB_FIRMWARE webfw={ADDR_FLASH_SECTOR_BAK,0};
				uint32 wcnt=0;						//�쳣����
				uint32 content_len=0;		
				char jumpto[20];unsigned char maxlen=20;
				char sub[10];
				//mydbg("raw:\r\n%s\r\n", &rx_buf[20]);
				mid((char*)http_request->URI,"Content-Length: ","\r\n",sub);		//�ҵ��ļ���
				content_len=atoi(sub);
				//mydbg("content_len=%ld \r\n", content_len);
				if(content_len > 1024*384){
					printf("�̼����Ȳ�����\r\n");
					disconnect(s);
					return;
				}
				mid((int8*)http_request->URI,"boundary=", "\r\n", (int8*)boundary);//�ҷָ��
				//myprintf("�̼��������ָ��boundary=%s\r\n",boundary);	
				webfw.pos1 = (char*)get_param_url(http_request->URI, jumpto, maxlen);		/*��ȡ�޸ĺ��IP��ַ*/
				if(webfw.pos1 == NULL){
					myerro("parm url not found\r\n");
					disconnect(s);
					return;
				}
				if(webfw.pos1- rx_buf < len-10){//Ŀǰ�յ������ݰ���������
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
							//myarg(webfw.remain_len);	//��������һ��δ���������
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
							webfw.total_w_len+= webfw.current_rxlen-webfw.tail_len;	//ĩβʱ�򣬼�ȥβ
						wcnt=0;	
						#if 0
						//W5300ֻ�����նˣ���Ҫ�ֶ����հ����߷��Ͷˣ�TCP�������ڵı仯�����ĳЩ�������HTML �հ� ����W5300��1�ֽڿհ�
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
					sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s",strlen(tx_buf),tx_buf);																				/*����http��Ӧ*/
					send(s, (uint8_t *)http_response, strlen((char *)http_response));	
					#endif
					disconnect(s);
					printf("�����ļ��������\r\n");
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
					sprintf((char *)http_response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s",strlen(tx_buf),tx_buf);																				/*����http��Ӧ*/
					send(s, (uint8_t *)http_response, strlen((char *)http_response));	
					disconnect(s);
					printf("�����ļ�����ʧ��,len=%d/%d\r\n", webfw.total_rxlen,webfw.data_len);
					WEB_LOG_UPDATE_ERR3(webfw.total_rxlen, webfw.data_len);
				}
				return;	
			}

#endif

void parm5_rpos_cgi(char *url)
{ 

	printf("rec parm5_rpos_cgi\n");
		return;
	char *p_content; 
	p_content = (char*)my_get_param_url(url);
	if(NULL == p_content){
		return;
	}
	http_sprintf_init();
	http_sprintf(HTML_CGI_JUMP, LOCAL_IP, C_PAGE_NAME);
	http_sprintf_send();
}







