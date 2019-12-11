
#include<stdio.h>    
#include<string.h>   
#include<stdlib.h>  

unsigned int a= 2000;
unsigned char get_a(void)
{
	return (unsigned int)a;
}

int main(int argc, char* argv[])
{
	unsigned int a;
	a= get_a();
	if(a > 1024)
		printf("a= %d千元\n", a/1024);
	else
		printf("a= %d元\n", a);
	

	return 0;
}


