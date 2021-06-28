#include<stdio.h> 
#include<string.h>   
#include<stdlib.h>   

//指定数组对齐 __attribute__((aligned(16)))

typedef struct st_tag1{
	char a;
	int b;
	char c;
}ST1;

typedef struct st_tag2{
	char a;
	int b;
	char c;
}__attribute__((packed))ST2;

#pragma pack(1) 
typedef struct st_tag3{
	char a;
	int b;
	char c;
}ST3;
#pragma pack()


int main(int argc, char *argv[]) 
{	
	int len1,len2,len3;
	ST1 myst1;
	ST2 myst2;
	ST3 myst3;
	len1= sizeof(myst1);
	len2= sizeof(myst2);
	len3= sizeof(myst3);
	printf("size=%d, %d, %d\r\n", len1,len2,len3);
	return 0;
}




