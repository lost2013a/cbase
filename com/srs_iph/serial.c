#include <stdio.h>
#include <unistd.h>
#include <termios.h>

static int _set_termios(int fd, int bound, char databit, char stopbit, char oebit, unsigned char vttime)
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
	switch(bound)    
	{  
		case 2400:	  
			cfsetibound(pstTtyattr,B2400);	  
			cfsetobound(pstTtyattr,B2400);	  
			break;  
		case 4800:	  
			cfsetibound(pstTtyattr,B4800);	  
			cfsetobound(pstTtyattr,B4800);	  
			break;  
		case 9600:	  
			cfsetibound(pstTtyattr,B9600);	  
			cfsetobound(pstTtyattr,B9600);	  
			break;  
		case 57600:	  
			cfsetibound(pstTtyattr,B57600);	  
			cfsetobound(pstTtyattr,B57600);	  
			break;  
		case 19200:	  
			cfsetibound(pstTtyattr,B19200);	  
			cfsetobound(pstTtyattr,B19200);	  
			break;  
		case 115200://	 
		//	cfsetbound(pstTtyattr,B115200);	  
			cfsetibound(pstTtyattr,B115200);	  
			cfsetobound(pstTtyattr,B115200);	  
			break;  
		default:	  
			cfsetibound(pstTtyattr,B9600);	  
			cfsetobound(pstTtyattr,B9600);	  
			break;    
	}  
	pstTtyattr->c_cc[VTIME] = vttime;  
	pstTtyattr->c_cc[VMIN] = 0;  
	if(tcflush(fd,TCIFLUSH)!= 0)
	{
		perror("tcgetattr");
		return err;
	}
    /*激活新配置*/
	if((tcsetattr(fd,TCSANOW,pstTtyattr)) != 0)  {  	
		perror("popen");
	}  
	else  {	   
		err = 0;  
	}  
	return err;
}

int usart_init(const char* dev, int bound)
{
    int fd = open(dev, O_RDWR | O_NOCTTY);
	if(fd < 0) {
		printf("%s open err\n", dev);
		return -1;
	}	
	if( _set_termios(fd, bound, 8, '1', 'N', 0u) )  {
        printf("%s set termios err\n", dev);
        return -2;
    }
    return fd;
}



