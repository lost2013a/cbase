#include<stdio.h> 
#include<string.h>   
#include<stdlib.h>  


int main(int argc, char *argv[]) 
{	
	char a=150;
	char b=200;
	char c=a+b;
	int d= (150+200)&0xff;
	printf("c=%d\n",c);
	printf("d=%d\n",d);
	return 0;
}




