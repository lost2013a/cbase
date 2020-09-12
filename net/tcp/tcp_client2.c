#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <netinet/tcp.h> 



#define SERVER_IP	"192.168.251.170"
#define SERVER_PORT	((uint16_t)2011)
#define BUFF_SIZE	(1024 * 4)

static int _select_version(int fd) 
{
	int ret = -1;
	int c_fd = fd;
	int ready_n;
	fd_set rset, wset;
	struct timeval tval;
	FD_ZERO(&rset);
	FD_SET(c_fd, &rset);
	wset = rset;
	tval.tv_sec = 5;	
	tval.tv_usec =0; 
	ready_n = select(c_fd + 1, &rset, &wset, NULL, &tval);
	switch (ready_n)
	{
		case -1:
			printf("socket select err\n");
			break;
		case 0:
			printf("socket select time out!\n");
			break;
		default:
			ret = 0;
			break;
	}
	return ret;
}

static int _socket_connected_detect(int fd)
{
    if (fd <= 0)
        return 0;
    struct tcp_info info;
    int len = sizeof(info);
    getsockopt(fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) & len);
    if ((info.tcpi_state == TCP_ESTABLISHED)) {
        return 1;
    } 
	else {
        printf("socket disconnected %d\n",info.tcpi_state);
        return 0;
    }
}



int main(int argc, char *argv[])
{
    int oldOption;
    int fd, rlen;
    char test_str[BUFF_SIZE]	= "tcp echo test";
    struct sockaddr_in	server_addr;

    
    
    fd	= socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket(2) error");
        return -1;
    }

    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(SERVER_PORT);
    server_addr.sin_addr.s_addr	= inet_addr(SERVER_IP);
        
    oldOption = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, oldOption | O_NONBLOCK);


    
	while(1){

        if(connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {  
            if(errno != EINPROGRESS){
            //连接没有立即返回，此时errno若不是EINPROGRESS，表明错误
                close(fd); 
                fd = -1;
            }
            else {
            	if(_select_version(fd) == -1)	{
					close(fd);
					fd = -1;
				} 	
           
            }  
        }
      	if(fd >=0){
			if(_socket_connected_detect(fd) == 1)
			{
				fcntl(fd, F_SETFL, oldOption);	
				printf("connect ok\n");
                break;
			}
			else{
				close(fd);
				fd = -1;
			}
		}	
		sleep(2);
	}

    return 0;
}
