
#include <stdio.h>    
#include <string.h>   
#include <stdlib.h>  
#include <unistd.h>



unsigned int a= 2000;
unsigned char get_a(void)
{
	return (unsigned int)a;
}

int* f(void)
{
    int v= 1;
    printf("在函数中：%p\n", &v);
    return &v;
}

int main(int argc, char* argv[])
{
	unsigned int a;
	a= get_a();
	if(a > 1024)
		printf("a= %d千元\n", a/1024);
	else
		printf("a= %d元\n", a);
	while(1){
        printf("在函数外：%p %p\n", f(), f());
        sleep(2);

    }

	return 0;
}


