#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#define SERVER_IP	"192.168.251.183"
#define SERVER_PORT	((uint16_t)5656)
#define BUFF_SIZE	(1024 * 10)

#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#define DBG_FILE "test.mp3"
#define max_dbg_len (1024*20)
static unsigned char dbg_wbuf[max_dbg_len];

static FILE *file_p;
static void write_flush(void);

static int write_file(unsigned char *buf, unsigned int len)
{
	if(fwrite(buf, 1, len ,file_p) < 0){
		printf("erro, log write failed\n");	
		return -1;
	}
	write_flush();
	return 0;
}

static void write_flush(void)
{
	fflush(file_p);
	fsync(fileno(file_p));
}


int log_file_open(void)
{
	int ret=0;
	file_p = fopen(DBG_FILE, "w+");	
	if(file_p == NULL){
		printf("log file open failed\n");
		ret= -1;
	}
	return ret;
}

void log_file_close(void)
{
	fclose(file_p);
}


void log_printf(char* fmt,...)  
{  
	unsigned int len;
	unsigned char* pbuf = dbg_wbuf;
	va_list ap;
	va_start(ap,fmt);
	len=vsnprintf((char*)pbuf, max_dbg_len, fmt,ap);
	va_end(ap);
	write_file(dbg_wbuf, len);
	write_flush();
}

void log_write(unsigned char *p_dta, unsigned int dtalen , char* head)
{  	
	if(head){
		if(fwrite((unsigned char *)head, 1, strlen(head) ,file_p) < 0){
			printf("erro, log write failed\n");	
			return;
		}
	}	
	write_file(p_dta, dtalen);
	write_flush();
}




int main(int argc, char *argv[])
{
    int conn_sock;
    unsigned char test_str[BUFF_SIZE];
    struct sockaddr_in	server_addr;
    socklen_t	addr_len	= sizeof(server_addr);
    fd_set	sockset;
 

    conn_sock	= socket(AF_INET, SOCK_DGRAM, 0);
    if (conn_sock < 0) {
        perror("socket(2) error");
        goto create_err;
    }

    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(SERVER_PORT);

	 if (bind(conn_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
        perror("bind(2) error");
        goto err;
    }
	 
   
    server_addr.sin_addr.s_addr	= inet_addr(SERVER_IP);
   	log_file_open();

   

while(1){
    while (true) {
        int num	= 0;

        FD_ZERO(&sockset);
        FD_SET(conn_sock, &sockset);
        num	= select(conn_sock + 1, &sockset, NULL, NULL, NULL);
        if (num < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("select(2) error");
                goto err;
            }
        } else if (num == 1) {
            if (FD_ISSET(conn_sock, &sockset)) {
                break;
            }
        } else if (num == 0) {
            fprintf(stderr, "%s\n", "Waiting for echo time out!");
            goto err;
        } else {
            fprintf(stderr, "%s\n", "Code should NOT reach here");
            goto err;
        }
    }

    (void)memset(test_str, 0, BUFF_SIZE);
	int len= recvfrom(conn_sock, test_str, BUFF_SIZE, 0,
                 (struct sockaddr *)&server_addr, &addr_len);
    if (len < 0) {
        perror("receive data error");
        goto err;
    }
 
    printf("[%d]", len-12);

	int i=0;
	for(i=0;  i< 12; i++){
		printf("%02X ", test_str[i]);
		
	}
	printf("\r\n");
    //if(len > 12)
    	log_write(&test_str[16], len-16, NULL);
    
}
    return EXIT_SUCCESS;

 err:
    close(conn_sock);
 create_err:
    fprintf(stderr, "client error");
    return EXIT_FAILURE;
}
