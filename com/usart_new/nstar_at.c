#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include "termios.h"




static int set_termios(int fd, long speed, unsigned char vttime);
int open_uart_fd(const char *dev_path, long speed, unsigned char vttime)
{
    int fd, ret;
    int bound = 115200;
    if(vttime >= 10) {
        printf("wrong: %d set vttime maybe too long\n", dev_path);
    }
    fd = open(dev_path, O_RDWR | O_NOCTTY);
    if(fd < 0) {
        printf("%s open faild\n", dev_path);
        return -1;
    }	
    ret = set_termios(fd, bound, vttime);	
    if(ret < 0) {
        printf("%s set faild\n", dev_path);
        return -2;
    }	
    printf("%s init ok, fd = %d\n", dev_path, fd);
    return fd;
}

static int set_termios(int fd, long speed, unsigned char vttime)
{  
	struct termios tty;
	struct termios *pstTtyattr= &tty;
	int err = -1;  
	memset(pstTtyattr, 0 , sizeof(struct termios));
	pstTtyattr->c_cflag |= CLOCAL | CREAD;  
	pstTtyattr->c_cflag &= ~CSIZE;  
	pstTtyattr->c_cflag |= CS8;	  
	pstTtyattr->c_cflag &= ~CSTOPB;			
//		case 'O':
//			//odd	 
//			pstTtyattr->c_cflag |= PARENB;	  
//			pstTtyattr->c_cflag |= (INPCK | ISTRIP);	  
//			pstTtyattr->c_cflag |= PARODD;	  
//			break;  
//		case 'E'://even	 
//			pstTtyattr->c_cflag |= PARENB;	  
//			pstTtyattr->c_cflag |= (INPCK | ISTRIP);	  
//			pstTtyattr->c_cflag &= ~PARODD;	  
//			break;  
//    case 'N'://noew
	pstTtyattr->c_cflag &= ~PARODD;	  
	switch(speed)    
	{  
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
	if(tcflush(fd,TCIFLUSH)!= 0){
		perror("tcgetattr");
		return err;
	}
	if((tcsetattr(fd,TCSANOW,pstTtyattr)) != 0) {
		perror("popen");
    }  
	else {
		err = 0;  
	}  
	return err;
}

int main(void)
{
    int fd = open_uart_fd("/dev/ttyUSB0", 115200, 1u);
    if(fd > 0) {
        while(1) {
            unsigned char buf[1024];
            //write(fd, "12\r\n", sizeof("12\r\n"));
            int len = read(fd, buf, 1024);
            if(len > 0) {
                printf("rec %d : %s\n", len, (char*)buf);
            }
            else {
                printf("no ack\r\n");
            }
            usleep(1000*200);
        }
    }
    close(fd);
    return 0;
}



