#include <stdio.h>

#define offsetof(type, member)   ((size_t)&(((type *)0)->member))
struct mystruct{
	int a;
	int b;
	char c;
	char d;
};

int main(void) 
{
	printf("1, %ld\n", offsetof(struct mystruct, d));
	
	
}