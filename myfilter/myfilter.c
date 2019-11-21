#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char myfilter(unsigned char sta)
{
#define FIILTER_DEPTH 9	//滤波级数，必须1-32
#define MASK (0xffffffff >> (32-FIILTER_DEPTH))
	static unsigned int old_sta=0;
	old_sta= (old_sta<<1) | (sta&0x1);
	if((old_sta & MASK) == MASK)
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