/*
  Copyright (c) 2009 Dave Gamble
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

/* Parse text to JSON, then render back to text, and print! */
static cJSON *json;
void doit(char *text)
{
	char *out;
	
	json=cJSON_Parse(text);
	if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
	else
	{
		out=cJSON_Print(json);
		//cJSON_Delete(json);
		printf("%s\n",out);
		free(out);
	}
}

//#define NSTAR_LOGIC_ADDR_LENGTH			6
//#define NSTAR_PHYSICS_ADDR_LENGTH		6
//struct _sys_status{
//	unsigned char physicsAddr[NSTAR_PHYSICS_ADDR_LENGTH];
//	unsigned char platformModeMask;
//	unsigned char initFlag;
//	struct _search_mode_change searchModeInfo;
//	struct _nstar_flash_data flashData;		
//	struct _platform platform;

//};



 
/* Read a file, parse, render back, etc. */
void dofile2(char *filename)
{
	cJSON *root, *name;
	FILE *f;long len;char *data;
	f=fopen(filename,"rb");
	fseek(f,0,SEEK_END);
	len=ftell(f);
	fseek(f,0,SEEK_SET);
	data=(char*)malloc(len+1);
	fread(data,1,len,f);
	fclose(f);

	root = cJSON_Parse(data);
	if (!root) {
	    printf("get root faild !\n");
	    printf("Error before: [%s]\n", cJSON_GetErrorPtr());
	}
	 
	//获取数据
	name = cJSON_GetObjectItem(root, "nb2");
	if (!name) {
	    printf("get name faild !\n");
	    printf("Error before: [%s]\n", cJSON_GetErrorPtr());
	}

	printf("name=%d\n",name->valueint);
	free(data);
}



/* Read a file, parse, render back, etc. */
void dofile(char *filename)
{
	FILE *f;long len;char *data;
	f=fopen(filename,"rb");
	fseek(f,0,SEEK_END);
	len=ftell(f);
	fseek(f,0,SEEK_SET);
	data=(char*)malloc(len+1);
	fread(data,1,len,f);
	fclose(f);
	doit(data);
	free(data);
}

static cJSON *root, *item1, *item2;

void Create_2(void)
{
	root  =	 cJSON_CreateObject();
	item1 =  cJSON_CreateObject();
	item2 =  cJSON_CreateObject();

	cJSON_AddItemToObject(root, "name", cJSON_CreateString("zx"));
	cJSON_AddItemToObject(root, "age", cJSON_CreateNumber(33));
	cJSON_AddItemToObject(root, "item1", item1);

	char *out= cJSON_Print(root);
	printf("node: %s\n", out);
	FILE* fp = fopen("tests/test2.txt", "w+"); 	
	fprintf(fp, out);   
	fclose(fp);

}

static void *cjson_malloc(size_t sz)
{
	printf("cjson_malloc %d\n", sz);
	return malloc(sz);
}

static void cjson_free(size_t sz)
{
	printf("cjson_free %d\n", sz);
	free(sz);
}

int main (int argc, const char * argv[]) {
	
	cJSON_Hooks hooks= {cjson_malloc, cjson_free};
	cJSON_InitHooks(&hooks);
	//Create_2();

	dofile("tests/test2.txt");

	return 0;
}
