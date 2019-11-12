#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char myfilter(unsigned char sta)
{
#define VAL_LEN 8	//必须1-32
#if (VAL_LEN > 32 || VAL_LEN < 1)
#define FIILTER_LEN 10
#else
#define FIILTER_LEN VAL_LEN	
#endif
	static unsigned int old_sta=0;
	unsigned int mask;
	sta&=0x1;
	old_sta= (old_sta<<1) | (sta&1);
	mask= 0xffffffff >> (32-FIILTER_LEN);
	if((old_sta & mask) == mask)
		return 1;
	else
		return 0;
}

int main(int argc, char **argv)
{
	int i=0,sta;
	for(i=0; i< 100; i++){
		if(i%10 == 0)
			sta=0;
		else
			sta=1;
		if(myfilter(sta))
			printf("err [%d]\n",i);

	}
	
	return 0;
}