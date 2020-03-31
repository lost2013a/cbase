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

#define SERVER_IP	"192.168.251.6"
#define SERVER_PORT	((uint16_t)2011)
#define BUFF_SIZE	(1024 * 4)

int main(int argc, char *argv[])
{
    int conn_sock, rlen;
    char test_str[BUFF_SIZE]	= "tcp echo test";
    struct sockaddr_in	server_addr;

    conn_sock	= socket(AF_INET, SOCK_STREAM, 0);
    if (conn_sock < 0) {
        perror("socket(2) error");
        goto create_err;
    }

    (void)memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family		= AF_INET;
    server_addr.sin_port		= htons(SERVER_PORT);
   
    server_addr.sin_addr.s_addr	= inet_addr(SERVER_IP);
    


	while(1){

		if (connect(conn_sock,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) >= 0) {
	        perror("connect(2) ok");
	        break;
   		 }
		else
			sleep(2);
				

	}
	printf("connect ok\n");
	if (write(conn_sock, "ok", strlen("ok")) < 0) {
        perror("send data error");
        goto err;
    }
	 
	while(1){

		rlen= recv(conn_sock, test_str, BUFF_SIZE, MSG_DONTWAIT);
		if(rlen > 0){
			printf("%s\n", test_str);
		}
	   	
	    printf("rlen =%d , %d\n", rlen, errno);
		if (write(conn_sock, "ok", strlen("ok")) < 0) {
        perror("send data error");
        goto err;
    }

		
		sleep(1);
		

	}
	
   


    return EXIT_SUCCESS;

 err:
    close(conn_sock);
 create_err:
    fprintf(stderr, "client error");
    return EXIT_FAILURE;
}
