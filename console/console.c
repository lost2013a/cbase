#include<stdio.h> 
#include<string.h>   
#include<stdlib.h>  

#include "console.h"



static unsigned int console_lv=1;

void console_lv_set(unsigned char ch)
{
	if(ch >= 32)
		return;
	console_lv |= 0x1<<ch;
}

void console_lv_clear(unsigned char ch)
{
	if(ch >= 32)
		return;
	console_lv &= ~(0x1<<ch);
}

unsigned int console_lv_get(unsigned char ch)
{
	if(ch >= 32)
		return 0;
	return console_lv&(0x1<<ch);
}

void argdbg(unsigned int arg, char *str)
{
	mydbg("%s=%d\r\n", str, arg);
}

void argdbgx(unsigned int arg, char *str)
{
	mydbg("%s=0x%x\r\n", str, arg);
}

int main(int argc, char *argv[]) 
{	

	mydbg("1st\n\n");
	console_lv_set(1);
	main_dbg("main_dbg !\r\n");
	iic_dbg("iic_dbg !\r\n");

	mydbg("2nd\n\n");
	console_lv_set(2);
	
	main_dbg("main_dbg !\r\n");
	iic_dbg("iic_dbg !\r\n");
	
	mydbg("3rd\n\n");
	console_lv_clear(1);
	console_lv_clear(2);
	main_dbg("main_dbg !\r\n");
	iic_dbg("iic_dbg !\r\n");
	int i= 16;
	myarg(i);
	myargx(i);
	return 0;
}




