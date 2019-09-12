#include "nstar_adt_httputil.h"
#include "nstar_adt_http_page.h"


#define JS_P1_E1 		"sip"
#define JS_P1_E2 		"dip"
#define JS_P1_E3 		"programIP"
#define JS_P1_E4 	    "logicID"
#define JS_P1_E5 	    "physID"

#define C_PAGE_NAME HTML_PAGE2_NAME

#define C_PAGE_BODY "<body>"\
"<ul>"\
"<li><a href='"HTML_PAGE1_NAME".html'>参数状态</a></li>"\
"<li><a href='"HTML_PAGE2_NAME".html' class='active'>网络设置</a></li>"\
"<li><a href='"HTML_PAGE3_NAME".html'>音频设置</a></li>"\
"<li><a href='"HTML_PAGE4_NAME".html'>信息日志</a></li>"\
"<li><a href='"HTML_PAGE5_NAME".html'>面板固件</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3> 网络设置</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<form id='frmSetting' method='POST' action='"C_PAGE_NAME".cgi'>"\
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




#define MIN(a,b) ( ((a)>=(b))?(b):(a))
#define MAX(a,b) ( ((a)>=(b))?(a):(b))

int str2ip(const char *str, unsigned char *ip, unsigned char len)
{
	char buf[4]={'\0'};
	int tmp;
	int i,j=0,nb=0;
	for(i= 0;i<len && nb<4;i++)									   
	{
		if (str[i]=='\0'||str[i]=='.'||str[i]==':'||j>=4)
		{
			tmp =atoi(buf);
			if (tmp<0||tmp>255){
				return 1;
			}
			ip[nb++]= tmp;	//set val
			memset(buf,0,4);
			j=0;
			continue;
		}
		buf[j]=str[i];	//copy
		j++;
	}
	return 0;
}

int str2mac(char *str, unsigned char *ip, unsigned char len)
{
	char buf[3]={'\0'};
	int tmp;
	int i,j=0,nb=0;
	for(i= 0;i<len && nb<6;i++)								   
	{
		if (str[i]=='\0'||str[i]==':'||j>=3)
		{
			sscanf(buf, "%x", &tmp);	
			if (tmp<0||tmp>255){
				return 1;
			}
			ip[nb++]= tmp;
			memset(buf,0,sizeof(buf));
			j=0;
			continue;
		}
		buf[j]=str[i];
		j++;
	}
	return 0;
}




static unsigned char _modify_pg_ip(char *p_dta)
{
/*命令通道*/
	unsigned short port=0;
	unsigned char ip[4], ret=0;
	//unsigned int u32_ip=0;
	char *p_port;
	if(0 == str2ip(p_dta, ip, MIN(16, strlen(p_dta)+1))){
		//u32_ip= (ip[0] << 24 | ip[1] << 16 | ip[2] << 8| ip[3]);
		p_port= strstr(p_dta, ":");
		if(NULL != p_port){
			p_port+=1;
			port= atoi(p_port);
			ret= 1;
		}
	}
	
	if(ip > 0 && port >0){
		
	}
	return ret;

}




static void _add_htm_element(unsigned char mode)
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

void parm2_pos_htm(unsigned char mode)
{
	if(mode == URI_REPOS_HTML)
		http_page_htm(C_PAGE_NAME, C_PAGE_BODY, _add_htm_element);
	else if(mode == URI_REPOS_JSON)
		http_page_json(C_PAGE_NAME, _add_htm_element);
}



void parm2_rpos_cgi(char *url)
{ 
	char *p_content; 
	p_content = (char*)my_get_param_url(url);
	if(NULL == p_content){
		return;
	}
	http_handle_parm(p_content, JS_P1_E1, (void*)_modify_pg_ip);
	http_sprintf_init();
	http_sprintf(HTML_CGI_JUMP, LOCAL_IP, C_PAGE_NAME);
	http_sprintf_send();
}





