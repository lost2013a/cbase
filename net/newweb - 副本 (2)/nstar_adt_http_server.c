#include "nstar_adt_http_server.h"

char c2d(unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c -'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c -'A';
	return (char)c;
}

void replacetochar(char * str,	char oldchar, char newchar)
{
	int x;
	for (x = 0; str[x]; x++) 
		if (str[x] == oldchar) str[x] = newchar;	
}

 /**
*@brief		  转化转义字符为ascii charater 
*@param		  url：需要转化网页地址
*@return	  无
*/
void unescape_http_url(char * url)
{
  int x, y;
  
  for (x = 0, y = 0; url[y]; ++x, ++y) 
  {
    if ((url[x] = url[y]) == '%') 
	{
      url[x] = c2d(url[y+1])*0x10+c2d(url[y+2]);
      y+=2;
    }
  }
  url[x] = '\0';
}

 /**
*@brief		  执行一个答复，如 html, gif, jpeg,etc.
*@param		  buf- 答复数据
*@param			type- 答复数据类型
*@param			len-  答复数据长度
*@return	  无
*/
void make_http_response_head(unsigned char * buf,char type,unsigned long len)
{
	char * head;
	char tmp[10];
	memset(buf,0x00,MAX_URI_SIZE); 
	/* 文件类型*/
	if 	(type == PTYPE_HTML) head = RES_HTMLHEAD_OK;
	else if (type == PTYPE_GIF)	head = RES_GIFHEAD_OK;
	else if (type == PTYPE_TEXT)	head = RES_TEXTHEAD_OK;
	else if (type == PTYPE_JPEG)	head = RES_JPEGHEAD_OK;
	else if (type == PTYPE_FLASH)	head = RES_FLASHHEAD_OK;
	else if (type == PTYPE_MPEG)	head = RES_MPEGHEAD_OK;
	else if (type == PTYPE_PDF)	head = RES_PDFHEAD_OK;

	sprintf(tmp,"%ld", len);	
	strcpy((char*)buf, head);
	strcat((char*)buf, tmp);
	strcat((char*)buf, "\r\n\r\n");
  //printf("%s\r\n", buf);
}

 /**
*@brief		  寻找一个MIME类型文件
*@param		  buf- MIME型文件
*@param		  type-数据类型
*@return	  无
*/
void find_http_uri_type(unsigned char * type, char * buf) 
{
	/* Decide type according to extention*/
	if 	(strstr(buf, ".pl"))				*type = PTYPE_PL;
	else if (strstr(buf, ".html") || strstr(buf,".htm"))	*type = PTYPE_HTML;
	else if (strstr(buf, ".gif"))				*type = PTYPE_GIF;
	else if (strstr(buf, ".text") || strstr(buf,".txt"))	*type = PTYPE_TEXT;
	else if (strstr(buf, ".jpeg") || strstr(buf,".jpg"))	*type = PTYPE_JPEG;
	else if (strstr(buf, ".swf")) 				*type = PTYPE_FLASH;
	else if (strstr(buf, ".mpeg") || strstr(buf,".mpg"))	*type = PTYPE_MPEG;
	else if (strstr(buf, ".pdf")) 				*type = PTYPE_PDF;
	else if (strstr(buf, ".cgi") || strstr(buf,".CGI"))	*type = PTYPE_CGI;
	else if (strstr(buf, ".js") || strstr(buf,".JS"))	*type = PTYPE_TEXT;	
	else if (strstr(buf, ".xml") || strstr(buf,".XML"))	*type = PTYPE_HTML;
	else 							*type = PTYPE_ERR;
}

 /**
*@brief		  解析每一个http响应
*@param		  request： 定义一个指针
*@return	  无
*/
void parse_http_request(st_http_request * request,unsigned char * buf)
{
	char * nexttok;
	nexttok = strtok((char*)buf," ");
	if(!nexttok)
	{
		request->METHOD = METHOD_ERR;
		return;
	}
	if(!strcmp(nexttok, "GET") || !strcmp(nexttok,"get"))
	{
		request->METHOD = METHOD_GET;
		nexttok = strtok(NULL," ");

	}
	else if (!strcmp(nexttok, "HEAD") || !strcmp(nexttok,"head"))	
	{
		request->METHOD = METHOD_HEAD;
		nexttok = strtok(NULL," ");

	}
	else if (!strcmp(nexttok, "POST") || !strcmp(nexttok,"post"))
	{
		nexttok = strtok(NULL,"\0");//20120316
		//nexttok = strtok(NULL," ");
		request->METHOD = METHOD_POST;

	}
	else
	{
		request->METHOD = METHOD_ERR;
	}	 
	if(!nexttok)
	{
		request->METHOD = METHOD_ERR; 			
		return;
	}
	strcpy(request->URI,nexttok); 					
}


unsigned char* get_param_url(char* uri, char* jump_to, unsigned char maxlen)
{
	char *p1, *p2;
	unsigned short content_len=0;
	if(!uri){
		printf("NULL\r\n");
		return NULL;
	}
	p1= strstr(uri,"Referer: http://");
	if(p1 == NULL){
		
		printf("no Referer\r\n");
		return NULL;
	}
	p1+= strlen("Referer: http://");
	p1= strstr(p1+1,"/");
	p2= strstr(p1+1,"\r\n");
	content_len= p2-p1;
	if(content_len < maxlen){
		memcpy(jump_to, p1, content_len);
		jump_to[content_len]= 0;
	}
	p1= strstr(uri,"\r\n\r\n");
	if(p1 == NULL){
		printf("url no parm part\r\n");
		return NULL;
	}
	p1 +=4;
	return (unsigned char*)p1;
}


unsigned char* my_get_param_url(char* uri)
{
	char *p1;
	if(!uri){
		printf("NULL\r\n");
		return NULL;
	}
	p1= strstr(uri,"Referer: http://");
	if(p1 == NULL){
		printf("no Referer\r\n");
		return NULL;
	}

	p1= strstr(p1,"\r\n\r\n");
	if(p1 == NULL){
		printf("url no parm part\r\n");
		return NULL;
	}
	p1 +=4;
	return (unsigned char*)p1;
}



char* get_http_param_value(unsigned char* buf, char* uri, char* param_name )
{
	char *p1,*p2;
	unsigned short len;
	p1= strstr(uri, param_name);
	if(p1){
		p1+= (strlen(param_name) + 1); //"="
		p2= strstr((char*)p1,"&");
		if(p2 == NULL) 
		{
			p2= p1+strlen((char*)p1);
		}
		len = p2-p1;
		if(len > 0)
		{
			buf[len]=0;
			strncpy((char*)buf, (char*)p1, len);
			unescape_http_url((char *)buf);			
			replacetochar((char *)buf, '+', ' ');	
		}
		else
			buf[0]=0;
	}
	else
		return NULL;
	return (char*)buf;		
}


char* http_mid(char* src, char* s1, char* s2, char* sub)
{
	
	char* sub1;
	char* sub2;
	unsigned short n;
	unsigned char res=0 ;
	sub1=strstr(src,s1);
	if(sub1 == NULL)
		return NULL;
	sub1+=strlen(s1);
	sub2=strstr(sub1,s2);
	if(sub2 == NULL)
		return NULL;
	n=sub2-sub1;
	strncpy(sub,sub1,n);
	sub[n]=0;
	return sub2;
}







