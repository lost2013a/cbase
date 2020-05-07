#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include "termios.h"
#include "zcx_msg.h"


#define DEV_NAME  "/dev/ttyUSB0"
#define DEFAULT_USART_BOUND 		 115200 		
#define UART_VTIME 					 0u 		/*单位:100ms*/

static int fm_uart_fd;
static int set_termios(int fd,long speed,char databit,char stopbit,char oebit, unsigned char vttime);

void arrry_print(unsigned char *data, unsigned int len)
{
	int i;
	for(i=0; i< len; i++){
		printf("%02X ", data[i]);
	}
	printf("\n");

}

void fm_uart_send(unsigned char *data, unsigned int len)
{
	if(write(fm_uart_fd, data, len) < 0)
		printf("%s write err\n", DEV_NAME);
}

int main (int argc, char *argv[])
{
	int fd;
	int len, i,ret;
	char buf[] = "hello ZLG!";
	int bound=DEFAULT_USART_BOUND;

	
	printf("start uart test\n");
	fd = open(DEV_NAME, O_RDWR | O_NOCTTY);
	if(fd < 0) {
		perror(DEV_NAME);
		printf("open %s faild\n", DEV_NAME);
		goto err;
	}

	ret = set_termios(fd, bound, 8, '1', 'N', UART_VTIME);	

	if(ret < 0) {
		printf("set %s faild\n", DEV_NAME);
		goto release;
	}
	else{
		fm_uart_fd= fd;
	}
	printf("%s set ok\n", DEV_NAME);
	
	_fm_zcx_init();
	while(1)
	{	
		len = read(fd, buf, sizeof(buf));
		if (len < 0) {
			printf("read error \n");
			break;
		}
		else if(len >0){
			for(i = 0; i<len; i++){
				_zcx_data_parse(buf[i]);
			}
		}
		usleep(10*1000);
	}

release:
	close(fd);
err:	
	return(0);
}


static int set_termios(int fd,long speed,char databit,char stopbit,char oebit, unsigned char vttime)
{  

	struct termios tty;
	struct termios *pstTtyattr= &tty;
	int err = -1;  
	memset(pstTtyattr, 0 , sizeof(struct termios));
	pstTtyattr->c_cflag |= CLOCAL | CREAD;  
	pstTtyattr->c_cflag &= ~CSIZE;  
	switch(databit)    
	{  
		case 7:	  
			pstTtyattr->c_cflag |= CS7;	  
			break;  
		case 8:	  
			pstTtyattr->c_cflag |= CS8;	  
		  break;  
		default:	  
			pstTtyattr->c_cflag |= CS8;	  
			break;    
	}  
	switch(stopbit)    
	{		
		case 1:			
			pstTtyattr->c_cflag &= ~CSTOPB;			
			break;		
		case 2:			
			pstTtyattr->c_cflag |= CSTOPB;		  
			break;		
		default:		  
			pstTtyattr->c_cflag &= ~CSTOPB;		  
			break;    
	}  
	switch(oebit)    
	{  
		case 'O':
			//odd	 
			pstTtyattr->c_cflag |= PARENB;	  
			pstTtyattr->c_cflag |= (INPCK | ISTRIP);	  
			pstTtyattr->c_cflag |= PARODD;	  
			break;  
		case 'E'://even	 
			pstTtyattr->c_cflag |= PARENB;	  
			pstTtyattr->c_cflag |= (INPCK | ISTRIP);	  
			pstTtyattr->c_cflag &= ~PARODD;	  
			break;  
		case 'N':	  
			pstTtyattr->c_cflag &= ~PARODD;	  
			break;  
		default:	  
			pstTtyattr->c_cflag &= ~PARODD;	  
			break;    
	}  
	switch(speed)    
	{  
		case 2400:	  
			cfsetispeed(pstTtyattr,B2400);	  
			cfsetospeed(pstTtyattr,B2400);	  
			break;  
		case 4800:	  
			cfsetispeed(pstTtyattr,B4800);	  
			cfsetospeed(pstTtyattr,B4800);	  
			break;  
		case 9600:	  
			cfsetispeed(pstTtyattr,B9600);	  
			cfsetospeed(pstTtyattr,B9600);	  
			break;  
		case 57600:	  
			cfsetispeed(pstTtyattr,B57600);	  
			cfsetospeed(pstTtyattr,B57600);	  
			break;  
		case 19200:	  
			cfsetispeed(pstTtyattr,B19200);	  
			cfsetospeed(pstTtyattr,B19200);	  
			break;  
		case 115200://	   
			cfsetispeed(pstTtyattr,B115200);	  
			cfsetospeed(pstTtyattr,B115200);	  
			break;  
		default:	  
			cfsetispeed(pstTtyattr,B9600);	  
			cfsetospeed(pstTtyattr,B9600);	  
			break;    
	}  
	pstTtyattr->c_cc[VTIME] = vttime;  
	pstTtyattr->c_cc[VMIN] = 0;  
	if(tcflush(fd,TCIFLUSH)!= 0)
	{
		perror("tcgetattr");
		return err;
	}
	if((tcsetattr(fd,TCSANOW,pstTtyattr)) != 0)   /*激活新配置*/ 
	{  	
		perror("popen");
	}  
	else  
	{	   
		err = 0;  
	}  
	return err;
}


